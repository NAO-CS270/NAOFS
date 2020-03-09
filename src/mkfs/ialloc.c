#include "dsk/blkfetch.h"
#include "mandsk/params.h"
#include "mkfs/diskParams.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/iNodeManager.h"
#include "inode/iNode.h"

#include <pthread.h>
#include <stdlib.h>

static const size_t iNodeNumsPerBlock = BLOCK_SIZE/INODE_ADDRESS_SIZE;

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

/* Returns first free iNode number, if any exists, `0` otherwise. Also removes
 * the iNode number from the input `iNodeList`.
 */
size_t checkAndGetFreeINode(iNodeListBlock *iNodeList) {
	size_t counter = 0;
	size_t freeINode = 0;

	while (counter < iNodeNumsPerBlock && freeINode == 0) {
		freeINode = (iNodeList -> iNodeNos)[counter];
		counter++;
	}
	if (counter == iNodeNumsPerBlock) {
	    // throw error
	    return -1;
	}

    (iNodeList -> iNodeNos)[counter-1] = 0;
	return freeINode;
}

void fetchFreeINodes(iNodeListBlock *iNodeList) {
	cacheNode *superBlockNode = getDiskBlockNode(SUPER_BLOCK, 0);

	superBlock *theSuperBlock = (superBlock *)malloc(sizeof(superBlock));
	makeSuperBlock(superBlockNode->dataBlock, theSuperBlock);

	size_t rememberedINode = theSuperBlock->rememberedINode;
	(theSuperBlock->rememberedINode) = searchINodes(rememberedINode, iNodeList);

	writeSuperBlock(theSuperBlock, superBlockNode->dataBlock);
	superBlockNode->header->delayedWrite = true;

	writeDiskBlockNode(superBlockNode);

	free(theSuperBlock);
}

size_t getNewINode(iNodeType fileType, mode_t fileMode, uid_t uid, gid_t gid) {
	pthread_mutex_lock(&iNodeListMutex);

	cacheNode *iNodeListBlockNode = getDiskBlockNode(INODE_LIST_BLOCK, 0);

	iNodeListBlock *iNodeList = (iNodeListBlock *)malloc(sizeof(iNodeListBlock));
	makeINodeListBlock(iNodeListBlockNode->dataBlock, iNodeList);

	size_t freeINode = checkAndGetFreeINode(iNodeList);

	if (freeINode == 0) {
		fetchFreeINodes(iNodeList);
		freeINode = checkAndGetFreeINode(iNodeList);
	}

	updateINodeData(freeINode, fileType, fileMode, uid, gid, 0, 0);
	writeINodeListBlock(iNodeList, iNodeListBlockNode->dataBlock);
	iNodeListBlockNode->header->delayedWrite = true;

	writeDiskBlockNode(iNodeListBlockNode);
	free(iNodeList);
	pthread_mutex_unlock(&iNodeListMutex);
	return freeINode;
}

