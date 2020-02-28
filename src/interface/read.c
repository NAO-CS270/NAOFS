#include "inode/inCoreiNode.h"
#include "mandsk/params.h"
#include "trav/directory.h"
#include "trav/namei.h"
#include "incoreInodeOps/iput.h"
#include "fdTable/fileTables.h"
#include "incoreInodeOps/bmap.h"
#include "dsk/blkfetch.h"
#include "utils/utils.h"

#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void fillDirectoryEntries(fuse_fill_dir_t filler, void *buffer, inCoreiNode *iNodePtr) {
	size_t entriesInDirectory = (iNodePtr->size) / DIRECTORY_ENTRY_SIZE;
	size_t entriesPerIteration = BLOCK_SIZE / DIRECTORY_ENTRY_SIZE;
	size_t entriesOffset = 0;
	size_t entriesRead;
	size_t counter;

	directoryEntry *entryBuffer = (directoryEntry *)malloc(entriesPerIteration * sizeof(directoryEntry));

	while (entriesOffset < entriesInDirectory) {
		memset(entryBuffer, 0, entriesPerIteration * sizeof(directoryEntry));
		entriesRead = searchINodeDirectoryEntries(iNodePtr, NULL, entriesOffset, entryBuffer, entriesPerIteration);

		counter = 0;
		while (counter < entriesRead) {
			filler(buffer, (entryBuffer + counter)->name, NULL, 0);
			counter++;
		}
		entriesOffset += entriesRead;
	}
	free(entryBuffer);
}

int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    inCoreiNode* iNodeOfPath = getFileINode(path, strlen(path));
    if(iNodeOfPath == NULL) {
        debug_print("iNode not found for path: %s\n", path);
        return -1;
    }

	fillDirectoryEntries(filler, buf, iNodeOfPath);

	iput(iNodeOfPath);
    return 0;
}

//TODO: Add pid verification
int validateFile(fileTableEntry *file, struct fuse_context *fuse_context) {
	if (file->inode->type == T_DIRECTORY) {
		return -EISDIR;
	}
	if (file->flags && O_RDONLY) {
		return 0;
	}
	return -EBADF;
}

void readInBlock(bmapResponse *bmapResp, char *buf, size_t size) {
	disk_block *blockPtr = (disk_block *)malloc(sizeof(disk_block));
	getDiskBlock(bmapResp->blockNumber, blockPtr);
	unsigned char *ptrIntoBlock = blockPtr->data;

	memcpy(buf, ptrIntoBlock + bmapResp->byteOffsetInBlock, size);

	free(blockPtr);
}

size_t readBytes(fileTableEntry *file, char *buf, size_t size, size_t offset) {
	inCoreiNode *fileINode = file->inode;
	size_t bytesRead = 0;
	ssize_t bytesLeft = min((ssize_t)size, (ssize_t)((ssize_t)fileINode->size - (ssize_t)offset));
	size_t lastByte = min(fileINode->size, size + offset);

	bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	
	while (bytesLeft > 0) { // TODO: offset doesn't get updated! //BUG
		int retValue = bmap(fileINode, offset, bmapResp, READ_MODE);
		if (retValue == -1) {
			printf("Reached end of file!!\n");
			break;
		}

		size_t bytesToReadFromBlock = min(bmapResp->bytesLeftInBlock, lastByte - bytesRead);
		readInBlock(bmapResp, buf + bytesRead, bytesToReadFromBlock);

		bytesRead += bytesToReadFromBlock;
		bytesLeft -= bytesToReadFromBlock;
		file->offset += bytesToReadFromBlock;
	}

	free(bmapResp);
	return bytesRead;
}

//TODO: Validate file
int readFile(char *buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context) {
	int retVal = 0;
	fileTableEntry *file = getFileDescriptor(fuse_context->pid, fi->fh, &retVal);
	if (retVal != 0) {
		debug_print("Some error for fd: %d\n", fi->fh);
		return retVal;
	}

	inCoreiNode *fileINode = file->inode;

	debug_print("Sleeping on lock for iNode %ld\n", fileINode->inode_number);
	pthread_mutex_lock(&(fileINode->iNodeMutex));
	debug_print("Acquired lock for iNode %ld\n", fileINode->inode_number);

	// retVal = validateFile(file, fuse_context);
	// if (retVal != 0) {
	// 	debug_print("Some error for fd: %d\n", fi->fh);
	// 	pthread_mutex_unlock(&(fileINode->iNodeMutex));
	// 	debug_print("Released lock for inode %ld\n", fileINode->inode_number);
	// 	return retVal;
	// }

	size_t bytesRead = readBytes(file, buf, size, offset);

	pthread_mutex_unlock(&(fileINode->iNodeMutex));
	debug_print("Released lock for inode %ld\n", fileINode->inode_number);

	return (int)bytesRead;
}
