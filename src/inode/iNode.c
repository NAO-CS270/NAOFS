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

	iNodePtr->inode_number = iNodeNum;
	return ;
}

size_t populateINodesIn(disk_block * blockPtr, size_t iNodeNum) {
	memset(blockPtr, 0, BLOCK_SIZE);

	iNode * iNodePtr;
	size_t iNodeCounter = iNodeNum;

	unsigned char *ptrIntoBlock = (blockPtr->data);
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	while (ptrIntoBlock < endOfBlock) {
		iNodePtr = (iNode *)malloc(iNodeSize);
		initializeINode(iNodePtr, iNodeCounter);

		memcpy(ptrIntoBlock, iNodePtr, iNodeSize);
		free(iNodePtr);

		ptrIntoBlock += iNodeSize;
		iNodeCounter++;
	}
	return iNodeCounter;
}

size_t makeINodeListBlock(disk_block * blockPtr, size_t *iNodeList, size_t listSize) {
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
