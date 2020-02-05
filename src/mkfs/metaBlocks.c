#include "specBlocks.h"
#include "dsk/mdisk.h"
#include "mandsk/params.h"

#include <stdlib.h>
#include <string.h>


iNodeListBlock *makeINodeListBlock(disk_block *blockPtr) {
	iNodeListBlock *listPtr = (iNodeListBlock *)malloc(sizeof(iNodeListBlock));

	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t iNodeNum = 0;
	size_t counter = 0;
	while (ptrIntoBlock != endOfBlock) {
		memcpy(&iNodeNum, ptrIntoBlock, INODE_ADDRESS_SIZE);

		(listPtr->iNodeNos)[counter] = iNodeNum;
		ptrIntoBlock += INODE_ADDRESS_SIZE;
		counter++;
	}

	return listPtr;
}
