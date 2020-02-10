#include "mkfs/ialloc.h"

static const size_t iNodeNumsPerBlock = BLOCK_SIZE/INODE_ADDRESS_SIZE;

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

/* Returns first free iNode number, if any exists, `0` otherwise. Also removes
 * the iNode number from the input `iNodeList`.
 */
size_t checkAndGetFreeINode(iNodeListBlock *iNodeList) {
	size_t counter = 0;
	size_t freeINode;

	while (counter < iNodeNumsPerBlock) {
		freeINode = (iNodeList -> iNodeNos)[counter];
		counter++;

		if (freeINode == 0) {
			continue;
		}
		(iNodeList -> iNodeNos)[counter-1] = 0;
		break;
	}
	return freeINode;
}

void fetchFreeINodes(iNodeListBlock *iNodeList) {
	disk_block *superBlockData = (disk_block *)malloc(BLOCK_SIZE);
	getDiskBlock(SUPER_BLOCK, superBlockData);

	superBlock *theSuperBlock = (superBlock *)malloc(sizeof(superBlock));
	makeSuperBlock(superBlockData, theSuperBlock);

	size_t rememberedINode = theSuperBlock->remembered_inode;
	(theSuperBlock->remembered_inode) = searchINodes(rememberedINode, iNodeList);

	writeSuperBlock(theSuperBlock, superBlockData);
	writeDiskBlock(SUPER_BLOCK, superBlockData);

	free(superBlockData);
	free(theSuperBlock);
}

size_t getNewINode() {
	pthread_mutex_lock(&iNodeListMutex);

	disk_block *iNodeListData = (disk_block *)malloc(BLOCK_SIZE);
	getDiskBlock(INODE_LIST_BLOCK, iNodeListData);

	iNodeListBlock *iNodeList = (iNodeListBlock *)malloc(sizeof(iNodeListBlock));
	makeINodeListBlock(iNodeListData, iNodeList);

	size_t freeINode = checkAndGetFreeINode(iNodeList);

	if (freeINode == 0) {
		fetchFreeINodes(iNodeList);
		freeINode = checkAndGetFreeINode(iNodeList);
	}

	markINodeFree(freeINode, T_REGULAR);
	writeINodeListBlock(iNodeList, iNodeListData);
	writeDiskBlock(INODE_LIST_BLOCK, iNodeListData);
	free(iNodeListData);
	free(iNodeList);

	pthread_mutex_unlock(&iNodeListMutex);
	return freeINode;
}

