#include <stdlib.h>
#include <string.h>

#include "inode/iNode.h"
#include "mkfs/iNodeManager.h"

void initializeINode(iNode *iNodePtr, size_t iNodeNum) {
	memset(iNodePtr, 0, sizeof(iNode));

	if (iNodeNum >= NUM_OF_INODES) {
		// throw appropriate error
		return ;
	}

	iNodePtr->device_number = 0;
	iNodePtr->inode_number = iNodeNum;
	iNodePtr->linksCount = 0;
	iNodePtr->type = T_FREE;
	iNodePtr->file_mode = 0;
	iNodePtr->owner_uid = 0;
	iNodePtr->group_uid = 0;
	return ;
}

/* Initializes and populates iNodes in the disk block at `blockPtr`. The created iNodes
 * start at iNode number `iNodeNum`. Move this function to `mkfs/metaBlocks.c` maybe.
 */
size_t populateINodesIn(disk_block * blockPtr, size_t iNodeNum) {
	memset(blockPtr, 0, BLOCK_SIZE);

	size_t iNodeStructSize = sizeof(iNode);
	iNode * iNodePtr = (iNode *)malloc(iNodeStructSize);
	size_t iNodeCounter = iNodeNum;

	unsigned char *ptrIntoBlock = (blockPtr->data);
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	while (ptrIntoBlock < endOfBlock) {
		initializeINode(iNodePtr, iNodeCounter);
		memcpy(ptrIntoBlock, iNodePtr, iNodeStructSize);
		
		ptrIntoBlock += INODE_SIZE;
		iNodeCounter++;
	}

	free(iNodePtr);
	return iNodeCounter;
}

size_t initINodeListBlock(disk_block * blockPtr, size_t *iNodeList, size_t listSize) {
	memset(blockPtr, 0, BLOCK_SIZE);
	size_t counter = 0;

	unsigned char *ptrIntoBlock = (blockPtr->data);
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	while (counter < listSize) {
		memcpy(ptrIntoBlock, iNodeList + counter, INODE_ADDRESS_SIZE);

		counter++;
		ptrIntoBlock += INODE_ADDRESS_SIZE;
	}

	size_t nextINode = 0;
	if (ptrIntoBlock == endOfBlock) {
		nextINode = *(iNodeList + counter - 1);
	}

	return nextINode;
}
