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
	
	disk_block *superBlockData = (disk_block *)malloc(BLOCK_SIZE);
	getDiskBlock(SUPER_BLOCK, superBlockData);

	superBlock *theSuperBlock = (superBlock *)malloc(sizeof(superBlock));
	makeSuperBlock(superBlockData, theSuperBlock);

	(theSuperBlock->rememberedINode) = iNodeNum;

	writeSuperBlock(theSuperBlock, superBlockData);
	writeDiskBlock(SUPER_BLOCK, superBlockData);

	free(theSuperBlock);
}

void freeINode(size_t iNodeNum) {
	pthread_mutex_lock(&iNodeListMutex);

	updateINodeData(iNodeNum, 0, 0, 0, 0, 0, 0);

	disk_block *iNodeListData = (disk_block *)malloc(BLOCK_SIZE);
	getDiskBlock(INODE_LIST_BLOCK, iNodeListData);

	iNodeListBlock *iNodeList = (iNodeListBlock *)malloc(sizeof(iNodeListBlock));
	makeINodeListBlock(iNodeListData, iNodeList);

	bool isSavedInList = saveInListIfPossible(iNodeNum, iNodeList);
	if (!isSavedInList) {
		rememberIfNeeded(iNodeNum, iNodeList);
		free(iNodeListData);
	}
	else {
		writeINodeListBlock(iNodeList, iNodeListData);
		writeDiskBlock(INODE_LIST_BLOCK, iNodeListData);
	}
	free(iNodeListData);
	free(iNodeList);

	pthread_mutex_unlock(&iNodeListMutex);
}

