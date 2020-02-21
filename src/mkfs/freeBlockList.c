#include "mkfs/freeBlockList.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t makeOneBlock(disk_block *blockPtr, size_t startNode) {
	memset(blockPtr, 0, BLOCK_SIZE);

	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t nodeCounter = startNode;

	while (ptrIntoBlock < endOfBlock && nodeCounter < NUM_OF_BLOCKS) {
		memcpy(ptrIntoBlock, &nodeCounter, BLOCK_ADDRESS_SIZE);
		nodeCounter++;
		ptrIntoBlock += BLOCK_ADDRESS_SIZE;
	}
	if (ptrIntoBlock == endOfBlock) {
		return nodeCounter-1;
	}
	return 0;
}

