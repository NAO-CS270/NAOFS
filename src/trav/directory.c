#include "directory.h"
#include "mandsk/params.h"
#include "inode/iNode.h"
#include "dsk/blkfetch.h"

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

