#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dsk/mdisk.h"
#include "mandsk/params.h"

static size_t blockSize = BLOCK_SIZE;
static size_t blockAddressSize = BLOCK_ADDRESS_SIZE;
static size_t numOfBlocks = NUM_OF_BLOCKS;

disk_block* makeOneBlock(disk_block *blockPtr, size_t startNode) {
	memset(blockPtr, 0, blockSize);

	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + blockSize;
	ptrIntoBlock += blockAddressSize;

	size_t nodeCounter = startNode;

	while (ptrIntoBlock < blockSize && nodeCounter < numOfBlocks) {
		memcpy(ptrIntoBlock, &nodeCounter, blockAddressSize);
		
		ptrIntoBlock += blockAddressSize;
		nodeCounter++;
	}
	if (nodeCounter < numOfBlocks) {
		memcpy(endOfBlock - BLOCK_SIZE, &nodeCounter, blockAddressSize);
	}

	return blockPtr;
}
