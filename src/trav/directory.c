#include "trav/directory.h"
#include "mandsk/params.h"
#include "inode/iNode.h"
#include "dsk/blkfetch.h"

#include <stdlib.h>
#include <string.h>
#include "inode/inCoreiNode.h"
#include "incoreInodeOps/bmap.h"
#include "incoreInodeOps/iget.h"


/* Sets data of `blockPtr` in `theBlock` appropriately
 */
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
void validateSearch(inCoreiNode *iNodePtr) {
	if ((iNodePtr->type != T_DIRECTORY) || (iNodePtr->mode & P_XUSR != P_XUSR)) {
		// TODO - throw an appropriate error
		printf("Directory traversal got iNode of non-directory\n");
	}
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

// fetches the directory entry table from the parent inode and adds a new entry
// at the end of the table. 0
void getAndUpdateDirectoryTable(inCoreiNode* parentInode, size_t newInodeNumber, char* filename) {
    // TODO: See what needs to be done when the validateSearch fails, also, how do we fetch the files here
    // validateSearch(parentInode);
    fetchInodeFromDisk(parentInode->inode_number, parentInode);
    // fetch the directory table from the disk
    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	bmap(parentInode, parentInode->size, bmapResp, READ_MODE);
    disk_block *blkPtr = (disk_block *) malloc(sizeof(disk_block));
    getDiskBlock(bmapResp->blockNumber, blkPtr);
    // fetch the directory table from the disk block
    directoryTable *dirData = (directoryTable *) malloc(sizeof(directoryTable));
    makeDirectoryTable(blkPtr, dirData);

    size_t counter;
    nameINodePair *iNodeData;

    for(counter = 0; counter < DIRECTORY_ENTRIES_IN_BLOCK ; counter++) {
        iNodeData = &(dirData->entries[counter]);
        if(NULL == iNodeData) {
            break;
        }
    }
    if (counter == DIRECTORY_ENTRIES_IN_BLOCK) {
        // TODO: Throw an error in this case
    }
    // counter is the new index at which the entry has to be made
    iNodeData = (nameINodePair*)malloc(sizeof(nameINodePair));
    iNodeData->iNodeNum = newInodeNumber;
    // TODO: strcpy
    memcpy(iNodeData->name, filename, FILENAME_SIZE);
    dirData->entries[counter] = *(iNodeData);

    // TODO: (Aarti) is this correct?
    memcpy(blkPtr, dirData, bmapResp->bytesLeftInBlock);
    writeMemoryDiskBlock(bmapResp->blockNumber, blkPtr);

	free(bmapResp);
    free(dirData);
    free(blkPtr);
}

// add "." and ".." entries to the directory entry table
// "." -> newInodeNumber
// ".." -> parentInodeNumber
void updateNewDirMetaData(inCoreiNode* inode, size_t newInodeNumber, size_t parentInodeNumber) {
    fetchInodeFromDisk(parentInodeNumber, inode);

    directoryTable *dirData = (directoryTable*)malloc(sizeof(directoryTable));
    memset(dirData, 0, sizeof(directoryTable));
    disk_block *blkPtr = (disk_block*)malloc(sizeof(disk_block));

    bmapResponse* bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	bmap(inode, inode->size, bmapResp, APPEND_MODE);
    getDiskBlock(bmapResp->blockNumber, blkPtr);
    dirData = makeDirectoryTable(blkPtr, dirData);

    // since this is called at the time of mkdir, the directory entry table will be empty.
    memset(dirData->entries[0].name, 0, FILENAME_SIZE); dirData->entries[0].name[0] = '.';
    dirData->entries[0].iNodeNum = newInodeNumber;

    //dirData->entries[1].name = "..";
    memcpy(dirData->entries[1].name, dirData->entries[0].name, FILENAME_SIZE); dirData->entries[0].name[1] = '.';
    dirData->entries[1].iNodeNum = parentInodeNumber;

    // TODO: (Aarti) is this correct?
    memcpy(blkPtr, dirData, bmapResp->bytesLeftInBlock);
    writeMemoryDiskBlock(bmapResp->blockNumber, blkPtr);

	free(bmapResp);
    free(dirData);
    free(blkPtr);
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
