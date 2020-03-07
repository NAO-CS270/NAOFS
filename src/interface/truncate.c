#include "interface/truncate.h"

#include "dsk/blkfetch.h"
#include "incoreInodeOps/bmap.h"
#include "incoreInodeOps/iNodeManager.h"
#include "incoreInodeOps/iput.h"
#include "inode/inCoreiNode.h"
#include "trav/namei.h"
#include "utils/utils.h"

#include <errno.h>
#include <fuse.h>
#include <stdlib.h>
#include <string.h>

void writeNullBytes(size_t blockNumber, size_t offset, size_t bytesToWrite) {
	disk_block *blockPtr = (disk_block *)malloc(sizeof(disk_block));
	getDiskBlock(blockNumber, blockPtr);

	unsigned char *ptrIntoBlk = blockPtr->data;
	memset(ptrIntoBlk + offset, 0, bytesToWrite);

	writeDiskBlock(blockNumber, blockPtr);

	free(blockPtr);
}

void increaseFileSizeBy (inCoreiNode* iNode, size_t size) {
	size_t bytesWritten = 0;
	size_t offset = iNode->size;
	bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	while (bytesWritten < size) {
		int retValue = bmap(iNode, offset, bmapResp, APPEND_MODE);
		size_t bytesToWrite = min(bmapResp->bytesLeftInBlock, size - bytesWritten);
		writeNullBytes(bmapResp->blockNumber, bmapResp->byteOffsetInBlock, bytesToWrite);
		offset += bytesToWrite;
		bytesWritten += bytesToWrite;
		updateINodeMetadata(iNode, bytesToWrite, iNode->linksCount);
	}

	free(bmapResp);
}

void decreaseFileSizeBy (inCoreiNode* iNode, size_t size) {
	size_t bytesReduced = 0;
	size_t offset = iNode->size - 1;
	bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
	while (bytesReduced < size) {
		int retValue = bmap(iNode, offset, bmapResp, READ_MODE);
		if ((size - bytesReduced) < (bmapResp->byteOffsetInBlock + 1)) {
			updateINodeMetadata(iNode, -(size - bytesReduced), iNode->linksCount);
			bytesReduced = size;
			break;
		}
		freeDataBlockInINode(iNode, bmapResp->blockNumber);
		updateINodeMetadata(iNode, -(bmapResp->byteOffsetInBlock + 1), iNode->linksCount);
		bytesReduced += bmapResp->byteOffsetInBlock + 1;
	}
	free(bmapResp);
}

void truncateINode (inCoreiNode* iNode, size_t size) {
	if (iNode->size == size) {
		return;
	}
	else if (iNode->size > size) {
		decreaseFileSizeBy(iNode, iNode->size - size);
	}
	else {
		increaseFileSizeBy(iNode, size - iNode->size);
	}
}

int truncateFile (const char *path, size_t size, struct fuse_context *fuse_context) {
	size_t pathLen = strlen(path);
	inCoreiNode *fileINode = getFileINode(path, pathLen);
	if (NULL == fileINode) {
		printf("File not found!!");
		return -ENOENT;
	}

	pthread_mutex_lock(&(fileINode->iNodeMutex));
	if (fileINode->type == T_DIRECTORY) {
		return -EISDIR;
	}

	truncateINode (fileINode, size);
	pthread_mutex_unlock(&(fileINode->iNodeMutex));

	iput(fileINode);
	return 0;
}

