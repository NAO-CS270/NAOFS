#include "mkfs/metaBlocks.h"
#include "dsk/mdisk.h"
#include "inode/iNode.h"

#include <string.h>

// For all functions, check that input pointers point to enough memory.

superBlock *makeSuperBlock(disk_block *blockPtr, superBlock *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t iNodeNum = 0;
	memcpy(&iNodeNum, ptrIntoBlock, INODE_ADDRESS_SIZE);

	theBlock->remembered_inode = iNodeNum;

	return theBlock;
}

disk_block *writeSuperBlock(superBlock *theBlock, disk_block *blockPtr) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	memcpy(ptrIntoBlock, &(theBlock->remembered_inode), INODE_ADDRESS_SIZE);

	return blockPtr;
}

freeDiskListBlock* makeFreeDiskListBlock(disk_block *blockPtr, freeDiskListBlock *theBlock) {
	unsigned char* ptrIntoBlock = blockPtr->data;
	unsigned char* endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t blkNum = 0;
	size_t counter = 0;
	while (ptrIntoBlock < endOfBlock) {
		memcpy(&blkNum, ptrIntoBlock, BLOCK_ADDRESS_SIZE);

		theBlock->blkNos[counter] = blkNum;
		counter++;
		ptrIntoBlock += BLOCK_ADDRESS_SIZE;
	}

	return theBlock;
}

disk_block* writeFreeDiskListBlock(freeDiskListBlock* theBlock, disk_block* blockPtr) {
	unsigned char* ptrIntoBlock = blockPtr->data;
	unsigned char* endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t blkNum = 0;
	size_t counter = 0;
	while (ptrIntoBlock < endOfBlock) {
		memcpy(ptrIntoBlock, &(theBlock->blkNos[counter]), BLOCK_ADDRESS_SIZE);

		counter++;
		ptrIntoBlock += BLOCK_ADDRESS_SIZE;
	}

	return blockPtr;
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

disk_block *writeINodeListBlock(iNodeListBlock *theBlock, disk_block *blockPtr) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t counter = 0;
	while (ptrIntoBlock < endOfBlock) {
		memcpy(ptrIntoBlock, &((theBlock->iNodeNos)[counter]), INODE_ADDRESS_SIZE);

		ptrIntoBlock += INODE_ADDRESS_SIZE;
		counter++;
	}

	return blockPtr;
}

iNodesBlock *makeINodesBlock(disk_block *blockPtr, iNodesBlock *theBlock) {
	size_t iNodeStructSize = sizeof(iNode);

	memcpy(theBlock->iNodesList, blockPtr->data, INODES_PER_BLOCK*iNodeStructSize);

	return theBlock;
}

disk_block *writeINodesBlock(iNodesBlock *theBlock, disk_block *blockPtr) {
	size_t iNodeStructSize = sizeof(iNode);

	memcpy(blockPtr->data, theBlock->iNodesList, INODES_PER_BLOCK*iNodeStructSize);
	return blockPtr;
}

