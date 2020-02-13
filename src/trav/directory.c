#include "trav/directory.h"
#include "mandsk/params.h"
#include "inode/iNode.h"
#include "dsk/blkfetch.h"
#include "inode/inCoreiNode.h"
#include "incoreInodeOps/bmap.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iNodeManager.h"

#include <stdlib.h>
#include <string.h>

/* Sets data of `blockPtr` in `theBlock` appropriately. */
directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t counter = 0;
	nameINodePair *fileData;
	while (ptrIntoBlock != endOfBlock) {
		fileData = &(theBlock->entries[counter]);
		
		memcpy(&(fileData->name), ptrIntoBlock, FILENAME_SIZE);
		ptrIntoBlock += FILENAME_SIZE;

		memcpy(&(fileData->iNodeNum), ptrIntoBlock, INODE_ADDRESS_SIZE);
		ptrIntoBlock += INODE_ADDRESS_SIZE;

		counter++;
	}
	return theBlock;
}

/* Should check that the `iNode` passed corresponds to a directory. Should also have the logic
 * of verifying whether the user has permissions to traverse through the directory. Should throw
 * appropriate exception if these checks fail.
 */
int validateSearch(inCoreiNode *iNodePtr) {
	if ((iNodePtr->type != T_DIRECTORY) || (iNodePtr->mode & P_XUSR != P_XUSR)) {
		// TODO - throw an appropriate error
		printf("Directory traversal got iNode of non-directory\n");
		return -1;
	}
	return 0;
}

/* Iterates through the directory `dataPtr` to find an entry corresponding to
 * `entryName`. Returns the corresponding `iNode` number for the entry if found
 * else returns `0`.
 */
size_t searchDirectory(directoryTable *dataPtr, char *entryName) {
	nameINodePair iNodeData;

	size_t counter;
	for (counter=0 ; counter<DIRECTORY_ENTRIES_IN_BLOCK ; counter++) {
		iNodeData = dataPtr->entries[counter];
		printf("subdir name %s , query %s strcmp %d \n", iNodeData.name, entryName, strcmp(entryName, iNodeData.name));
		if (0 == strcmp(entryName, iNodeData.name)) {
			return iNodeData.iNodeNum;
		}
	}
	return 0;
}

/* Considering passed `iNodePtr` as corresponding to a directory, fetches its disk blocks
 * and finds the iNode of `entryName` in the directory. Returns `0` if not found.
 */
size_t findINodeInDirectory(inCoreiNode *iNodePtr, char *entryName) {
	validateSearch(iNodePtr);

	directoryTable *dirData = (directoryTable *)malloc(sizeof(directoryTable));
	disk_block *blockPtr = (disk_block *)malloc(sizeof(disk_block));
	size_t foundINode = 0;
	
	size_t blockNum;
	size_t offsetIntoDirectory = 0;

	bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse *));

	while (true) {
		size_t retValue = bmap(iNodePtr, offsetIntoDirectory, bmapResp, READ_MODE);
		if (retValue == -1) {
		    foundINode = 0;
			break;
		}

		printf("Searching directory block num - %ld\n", bmapResp -> blockNumber);
		getDiskBlock(bmapResp -> blockNumber, blockPtr);
		makeDirectoryTable(blockPtr, dirData);

		foundINode = searchDirectory(dirData, entryName);

		if (foundINode != 0) {
			break;
		}
		offsetIntoDirectory += BLOCK_SIZE;
	}
	free(bmapResp);
	free(dirData);
	free(blockPtr);

	return foundINode;
}

void addDirectoryEntry(size_t blockNum, size_t blockOffset, const char *filename, size_t iNodeNum) {
	disk_block *blockPtr = (disk_block *) malloc(sizeof(disk_block));
	getDiskBlock(blockNum, blockPtr);

	unsigned char *ptrIntoBlock = blockPtr->data;
	ptrIntoBlock += blockOffset;
	memcpy(ptrIntoBlock, filename, FILENAME_SIZE);
	ptrIntoBlock += FILENAME_SIZE;
	memcpy(ptrIntoBlock, &(iNodeNum), INODE_ADDRESS_SIZE);

	writeDiskBlock(blockNum, blockPtr);
	free(blockPtr);
}

/* Fetches the directory disk block of `parentINode` and adds a new entry for `filename`
 * at the end of the table.
 */
int getAndUpdateDirectoryTable(inCoreiNode* parentINode, size_t newInodeNumber, char* filename) {
    int retValue = validateSearch(parentINode);
	if (retValue == -1) {
		printf("Incorrect permissions or file type of parent. Can't create directory.\n");
		return -1;
	}

    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	retValue = bmap(parentINode, parentINode->size, bmapResp, APPEND_MODE);
	if (retValue == -1) {
		printf("BMap failed.\n");
		return -1;
	}
    
	addDirectoryEntry(bmapResp->blockNumber, bmapResp->byteOffsetInBlock, filename, newInodeNumber);
	updateINodeMetadata(parentINode, DIRECTORY_ENTRY_SIZE);

	free(bmapResp);
}

/* Adds entries for `.` and `..` in `iNode` with parent iNode `parentINodeNumber`.*/
int updateNewDirMetaData(inCoreiNode* iNode, size_t parentINodeNumber) {
    bmapResponse* bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));

	int retValue = bmap(iNode, iNode->size, bmapResp, APPEND_MODE);
	if (retValue == -1) {
		printf("BMap failed.\n");
		return -1;
	}

	addDirectoryEntry(bmapResp->blockNumber, bmapResp->byteOffsetInBlock, ".", iNode->inode_number);
	addDirectoryEntry(bmapResp->blockNumber, (bmapResp->byteOffsetInBlock) + DIRECTORY_ENTRY_SIZE, "..", parentINodeNumber);
	updateINodeMetadata(iNode, 2*DIRECTORY_ENTRY_SIZE);

	free(bmapResp);
}

directoryTable* getDirectoryEntries(inCoreiNode* inode) {
    directoryTable *dirTable = (directoryTable*)malloc(sizeof(directoryTable));
    disk_block *blkPtr = (disk_block*)malloc(sizeof(disk_block));

    bmapResponse* bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	bmap(inode, inode->size, bmapResp, READ_MODE);
    blkPtr = getDiskBlock(bmapResp->blockNumber, blkPtr);
    dirTable = makeDirectoryTable(blkPtr, dirTable);

	free(bmapResp);
    return dirTable;
}

