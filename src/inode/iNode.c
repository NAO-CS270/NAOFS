#include <stdlib.h>
#include <string.h>

#include "iNode.h"
#include "mandsk/params.h"
#include "dsk/mdisk.h"

static const size_t iNodeSize = INODE_SIZE;			// TODO - Update

void initializeINode(iNode *iNodePtr, size_t iNodeNum) {
	memset(iNodePtr, 0, iNodeSize);

	if (iNodeNum >= NUM_OF_INODES) {
		// throw appropriate error
		return ;
	}

	iNodePtr->device_number = 0;
	iNodePtr->inode_number = iNodeNum;
	iNodePtr->linksCount = 0;
	iNodePtr->type = T_REGULAR;
	iNodePtr->mode = P_RUSR | P_WUSR | P_RGRP;
	iNodePtr->owner_uid = 0;
	iNodePtr->group_uid = 0;
	return ;
}

/* Initializes and populates iNodes in the disk block at `blockPtr`. The created iNodes
 * start at iNode number `iNodeNum`. Move this function to `mkfs/metaBlocks.c` maybe.
 */
size_t populateINodesIn(disk_block * blockPtr, size_t iNodeNum) {
	memset(blockPtr, 0, BLOCK_SIZE);

	iNode * iNodePtr = (iNode *)malloc(iNodeSize);
	size_t iNodeCounter = iNodeNum;

	unsigned char *ptrIntoBlock = (blockPtr->data);
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	while (ptrIntoBlock < endOfBlock) {
		initializeINode(iNodePtr, iNodeCounter);
		memcpy(ptrIntoBlock, iNodePtr, iNodeSize);

		ptrIntoBlock += iNodeSize;
		iNodeCounter++;
	}

	free(iNodePtr);
	return iNodeCounter;
}

size_t initINodeListBlock(disk_block * blockPtr, size_t *iNodeList, size_t listSize) {
	memcpy(blockPtr, 0, BLOCK_SIZE);
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
