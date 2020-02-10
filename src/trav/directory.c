#include "directory.h"
#include "../mandsk/params.h"
#include "../inode/iNode.h"
#include "../dsk/blkfetch.h"

#include <stdlib.h>
#include <string.h>

/* Sets data of `blockPtr` in `theBlock` appropriately
 */
directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t counter = 0;
	nameINodePair *fileData;
	while (ptrIntoBlock != endOfBlock) {
		fileData = &(theBlock->entries[counter]);
		
		memcpy(fileData->name, ptrIntoBlock, FILENAME_SIZE);
		ptrIntoBlock += FILENAME_SIZE;

		memcpy(fileData->iNodeNum, ptrIntoBlock, INODE_ADDRESS_SIZE);
		ptrIntoBlock += INODE_ADDRESS_SIZE;

		counter++;
	}
	return theBlock;
}

/* Should check that the `iNode` passed corresponds to a directory. Should also have the logic
 * of verifying whether the user has permissions to traverse through the directory. Should throw
 * appropriate exception if these checks fail.
 */
void validateSearch(iNode *iNodePtr) {
	if ((iNodePtr->type != T_DIRECTORY) || (iNodePtr->mode & P_XUSR != P_XUSR)) {
		// TODO - throw an appropriate error
	}
}

/* Iterates through the directory `dataPtr` to find an entry corresponding to
 * `entryName`. Returns the corresponding `iNode` number for the entry if found
 * else returns `0`.
 */
size_t searchDirectory(directoryTable *dataPtr, char *entryName) {
	nameINodePair iNodeData;

	size_t counter;
	for (counter=0 ; counter<ENTRIES_PER_BLOCK ; counter++) {
		iNodeData = dataPtr->entries[counter];

		if (0 == strncmp(entryName, iNodeData.name, FILENAME_SIZE)) {
			break;
		}
	}

	if (ENTRIES_PER_BLOCK == counter) {
		return 0;
	}
	return iNodeData.iNodeNum;
}

/* Fetches the disk blocks corresponding to the file `iNodePtr` and finds the
 * iNode of `entryName`. Returns `0` if not found.
 */
size_t findINodeInDirectory(iNode *iNodePtr, char *entryName) {
	validateSearch(iNodePtr);

	directoryTable *dirData = (directoryTable *)malloc(sizeof(directoryTable));
	disk_block *blockPtr = (disk_block *)malloc(sizeof(disk_block));
	size_t foundINode = 0;
	
	size_t counter;
	for (counter=0 ; counter<BLOCKS_IN_INODE ; counter++) {
		size_t blockNum = iNodePtr->dataBlockNums[counter];
		getDiskBlock(blockNum, blockPtr);
		makeDirectoryTable(blockPtr, dirData);

		foundINode = searchDirectory(dirData, entryName);

		if (foundINode != 0) {
			break;
		}
	}
	free(dirData);
	free(blockPtr);

	return foundINode;
}

void getAndUpdateDirectoryTable(inCoreiNode* inode, size_t newInodeNumber, char* filename) {
    // TODO: See what needs to be done when the validateSearch fails
    validateSearch(inode);

    // fetch the directory table from the disk
    bmapResponse* bmapResp = bmap(parentInode, parentInode->disk_iNode->size);
    directoryTable *dirData = (directoryTable*)malloc(sizeof(directoryTable));
    disk_block *blkPtr = (disk_block*)malloc(sizeof(disk_block));
    // fetch the directory table from the disk block
    getDiskBlock(blockNum, blkPtr);
    makeDirectoryTable(blkPtr, dirData);

    size_t counter;
    nameINodePair iNodeData;

    for(counter = 0; counter < ENTRIES_PER_BLOCK; counter++) {
        iNodeData = dataPtr->entries[counter];
        if(NULL == iNodeData) {
            break;
        }
    }
    if (counter == ENTRIES_PER_BLOCK) {
        // TODO: Throw an error in this case
    }
    // counter is the new index at which the entry has to be made
    iNodeData = (nameINodePair*)malloc(sizeof(nameINodePair));
    iNodeData.iNodeNum = newInodeNumber;
    iNodeData.name = filename;

    dataPtr->entries[counter] = iNodeData;
    // TODO: Is this correct?
    *(blkPtr) = *(dataPtr);
    writeMemoryDiskBlock(bmapResp->blockNumber, blkPtr);

    free(dataPtr);
    free(blkPtr);
}

