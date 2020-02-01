#include <stdlib.h>

#include "iNode.h"
#include "mandsk/params.h"
#include "dsk/mdisk.h"

static const size_t iNodeAddressSize = INODE_ADDRESS_SIZE;

static const size_t blockSize = BLOCK_SIZE;
static const size_t numOfBlocks = NUM_OF_BLOCKS;
static const size_t iNodeSize = INODE_SIZE;			// TODO - Update
static const size_t numOfINodes = NUM_OF_INODES;

void initializeINode(iNode *iNodePtr, size_t iNodeNum) {
	memset(iNodePtr, 0, iNodeSize);

	if (iNodeNum >= numOfINodes) {
		// throw appropriate error
		return ;
	}

	iNodePtr->inode_number = iNodeNum;
	return ;
}

size_t populateINodesIn(disk_block * blockPtr, size_t iNodeNum) {
	memset(blockPtr, 0, blockSize);

	iNode * iNodePtr;
	size_t iNodeCounter = iNodeNum;

	unsigned char *ptrIntoBlock = (blockPtr->data);
	unsigned char *endOfBlock = ptrIntoBlock + blockSize;

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
	memcpy(blockPtr, 0, blockSize);
	size_t counter = 0;

	unsigned char *ptrIntoBlock = (blockPtr->data);
	unsigned char *endOfBlock = ptrIntoBlock + blockSize;

	while (counter < listSize) {
		memcpy(ptrIntoBlock, iNodeList + counter, iNodeAddressSize);

		counter++;
		ptrIntoBlock += iNodeAddressSize;
	}

	size_t nextINode = 0;
	if (ptrIntoBlock == endOfBlock) {
		nextINode = *(iNodeList + counter - 1);
	}

	return nextINode;
}
