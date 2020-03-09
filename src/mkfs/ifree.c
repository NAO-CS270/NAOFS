#include "mkfs/ifree.h"
#include "mkfs/diskParams.h"
#include "mkfs/metaBlocks.h"
#include "dsk/blkfetch.h"
#include "mandsk/params.h"
#include "mkfs/iNodeManager.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

static const size_t iNodeNumsPerBlock = BLOCK_SIZE/INODE_ADDRESS_SIZE;

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

/* Tries to save `iNodeNum` in `iNodeList` if there is space. Returns `true` if
 * saved, else `false`.
 */
bool saveInListIfPossible(size_t iNodeNum, iNodeListBlock *iNodeList) {
	size_t counter = 0;
	size_t ptrINodeNum;

	while (counter < iNodeNumsPerBlock) {
		ptrINodeNum = (iNodeList -> iNodeNos)[counter];

		if (ptrINodeNum != 0) {
			if (counter > 0) {
				(iNodeList ->iNodeNos)[counter-1] = iNodeNum;
			}
			break;
		}
		counter++;
	}

	return (counter != 0);
}

void rememberIfNeeded(size_t iNodeNum, iNodeListBlock *iNodeList) {
	size_t lastINodeNum = (iNodeList -> iNodeNos)[iNodeNumsPerBlock-1];

	if (lastINodeNum < iNodeNum) {
		return ;
	}
	
	cacheNode *superBlockNode = getDiskBlockNode(SUPER_BLOCK, 0);

	superBlock *theSuperBlock = (superBlock *)malloc(sizeof(superBlock));
	makeSuperBlock(superBlockNode->dataBlock, theSuperBlock);

	(theSuperBlock->rememberedINode) = iNodeNum;

	writeSuperBlock(theSuperBlock, superBlockNode->dataBlock);
	superBlockNode->header->delayedWrite = true;

	writeDiskBlockNode(superBlockNode);

	free(theSuperBlock);
}

void freeINode(size_t iNodeNum) {
	pthread_mutex_lock(&iNodeListMutex);

	updateINodeData(iNodeNum, 0, 0, 0, 0, 0, 0);

	cacheNode *iNodeListBlockNode = getDiskBlockNode(INODE_LIST_BLOCK, 0);

	iNodeListBlock *iNodeList = (iNodeListBlock *)malloc(sizeof(iNodeListBlock));
	makeINodeListBlock(iNodeListBlockNode->dataBlock, iNodeList);

	bool isSavedInList = saveInListIfPossible(iNodeNum, iNodeList);
	if (!isSavedInList) {
		rememberIfNeeded(iNodeNum, iNodeList);
	}
	else {
		writeINodeListBlock(iNodeList, iNodeListBlockNode->dataBlock);
		iNodeListBlockNode->header->delayedWrite = true;
		writeDiskBlockNode(iNodeListBlockNode);
	}
	free(iNodeList);

	pthread_mutex_unlock(&iNodeListMutex);
}

