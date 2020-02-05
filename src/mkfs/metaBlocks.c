#include "metaBlocks.h"
#include "dsk/mdisk.h"
#include "mandsk/params.h"
#include "inode/iNode.h"

#include <stdlib.h>
#include <string.h>

// For all functions, check that input pointers point to enough memory.

superBlock *makeSuperBlock(disk_block *blockPtr, superBlock *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t iNodeNum = 0;
	memcpy(&iNodeNum, ptrIntoBlock, INODE_ADDRESS_SIZE);

	theBlock->rememberedINodeNum = iNodeNum;

	return theBlock;
}

iNodeListBlock *makeINodeListBlock(disk_block *blockPtr, iNodeListBlock *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t iNodeNum = 0;
	size_t counter = 0;
	while (ptrIntoBlock < endOfBlock) {
		memcpy(&iNodeNum, ptrIntoBlock, INODE_ADDRESS_SIZE);

		(theBlock->iNodeNos)[counter] = iNodeNum;
		ptrIntoBlock += INODE_ADDRESS_SIZE;
		counter++;
	}

	return theBlock;
}

iNodesBlock *makeINodesBlock(disk_block *blockPtr, iNodesBlock *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	iNode *iNodePtr = theBlock->iNodesList;
	size_t iNodeStructSize = sizeof(iNode);			// TODO: This should go away once structure is fixed.

	while (ptrIntoBlock < endOfBlock) {
		memcpy(iNodePtr, ptrIntoBlock, iNodeStructSize);

		ptrIntoBlock += INODE_SIZE;
		iNodePtr++;									// TODO: Make sure this increases appropriately.
	}

	return theBlock;
}
