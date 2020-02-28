#include "trav/directory.h"
#include "mandsk/params.h"
#include "inode/iNode.h"
#include "dsk/blkfetch.h"
#include "inode/inCoreiNode.h"
#include "incoreInodeOps/bmap.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iNodeManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Sets data of `blockPtr` in `theBlock` appropriately. */
directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock) {
	unsigned char *ptrIntoBlock = blockPtr->data;
	unsigned char *endOfBlock = ptrIntoBlock + BLOCK_SIZE;

	size_t counter = 0;
	directoryEntry *fileData;
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
	if (!S_ISDIR(iNodePtr->file_mode) || (iNodePtr->file_mode & S_IXUSR != S_IXUSR)) {
		// TODO - throw an appropriate error
		printf("Directory traversal got iNode of non-directory\n");
		return -1;
	}
	return 0;
}

/* Iterates through the block `blockPtr` as it were a directory, starting at `offset`. This operates in two modes -
 * search and collect depending on whether `entryName` is `NULL` or not. For search it adds only the directory entry
 * it's searching for, if found to `entryBuffer`. In collect mode, it adds all directory entries to `entryBuffer`.
 * Return value is the number of elements added to `entryBuffer`.
 */
size_t searchBlockDirectoryEntries(disk_block *blockPtr, char *entryName, size_t offset, directoryEntry *entryBuffer, size_t numOfEntries) {
	size_t found = 0;

	size_t counter = 0;
	unsigned char *ptrIntoBlock = blockPtr->data;
	ptrIntoBlock += offset;

	while (counter < numOfEntries) {
		if (entryName != NULL && strcmp(ptrIntoBlock, entryName) == 0) {
			memcpy(entryBuffer->name, ptrIntoBlock, FILENAME_SIZE);
			memcpy(&(entryBuffer->iNodeNum), ptrIntoBlock+FILENAME_SIZE, INODE_ADDRESS_SIZE);
			found = 1;
			break;
		}
		else if (entryName == NULL) {
			memcpy(entryBuffer[counter].name, ptrIntoBlock, FILENAME_SIZE);
			memcpy(&(entryBuffer[counter].iNodeNum), ptrIntoBlock+FILENAME_SIZE, INODE_ADDRESS_SIZE);
		}

		ptrIntoBlock += DIRECTORY_ENTRY_SIZE;
		counter++;
	}

	return (entryName==NULL) ? counter : found;
}

size_t checkInBlock(size_t tillEndOfFile, bmapResponse *bmapResp, char *entryName, directoryEntry *entryBuffer) {
	disk_block *blockPtr = (disk_block *)malloc(sizeof(disk_block));
	getDiskBlock(bmapResp->blockNumber, blockPtr);
		
	size_t entriesInBlock = (bmapResp->bytesLeftInBlock) / DIRECTORY_ENTRY_SIZE;
	if (bmapResp->bytesLeftInBlock > tillEndOfFile) {
		entriesInBlock = tillEndOfFile / DIRECTORY_ENTRY_SIZE;
	}

	size_t retValue = searchBlockDirectoryEntries(blockPtr, entryName, bmapResp->byteOffsetInBlock, entryBuffer, entriesInBlock);
	free(blockPtr);
	return retValue;
}

/* Considering passed `iNodePtr` as corresponding to a directory, it fetches the disk blocks starting after `offset` entries.
 * This operates in two modes - search and collect depending on whether `entryName` is `NULL` or not. For search it adds
 * only the directory entry it's searching for, if found to `entryBuffer`. In collect mode, it adds atmost `numOfEntries`.
 * Return value is the number of elements added to `entryBuffer`.
 */
int searchINodeDirectoryEntries(inCoreiNode *iNodePtr, char *entryName, size_t offset, directoryEntry *entryBuffer, size_t numOfEntries) {
	int retValue = validateSearch(iNodePtr);
	if (retValue == -1) {
		printf("Not permitted to read directory!!\n");
		return -1;
	}

	bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	size_t directoryOffset = offset*DIRECTORY_ENTRY_SIZE;
	size_t entriesReadTillNow = 0;

	while (entryName != NULL || entriesReadTillNow < numOfEntries) {
	        printf("numberOfEntries: %ld , entriesReadTillNow: %ld", numOfEntries, entriesReadTillNow);
		retValue = bmap(iNodePtr, directoryOffset, bmapResp, READ_MODE);
		if (retValue == -1) {
			printf("Reached end of directory!!\n");
			break;
		}

		size_t entriesRead = checkInBlock(iNodePtr->size - directoryOffset, bmapResp, entryName, entryBuffer + entriesReadTillNow);
		entriesReadTillNow += entriesRead;
		
		if (entryName != NULL && entriesRead == 1) {
			break;
		}
		directoryOffset += (DIRECTORY_ENTRIES_IN_BLOCK*DIRECTORY_ENTRY_SIZE);
	}

	free(bmapResp);
	return entriesReadTillNow;
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

