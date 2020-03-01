#include "interface/open.h"
#include "fdTable/fileTables.h"
#include "inode/inCoreiNode.h"
#include "trav/namei.h"
#include "utils/utils.h"

#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TODO: Check if O_APPEND actually sets file offset to be size. Doesn't think it does now
size_t calculateFileOffset (inCoreiNode *iNode, int flags) {
	size_t offset = 0;
	if (flags && O_APPEND) {
		offset = iNode->size;
	}
	return offset;
}

// O_APPEND will always write at the end of the file, irrescpective of the file pointer position
// Creation (O_CREAT, O_EXCL, O_NOCTTY) flags will be filtered out / handled by the kernel
int openFile(const char* path, struct fuse_file_info *fi, struct fuse_context *fuse_context) {
	size_t pathLen = strlen(path);
	inCoreiNode *fileINode = getFileINode(path, pathLen);
	if (NULL == fileINode) {
		printf("File not found!!");
		return -ENOENT;
	}

	debug_print("Sleeping on lock for iNode %ld\n", fileINode->inode_number);
	pthread_mutex_lock(&(fileINode->iNodeMutex));
	printf("Acquired lock for iNode %ld\n", fileINode->inode_number);

	size_t offset = calculateFileOffset(fileINode, fi->flags);
	int fd = createAndGetFileDescriptor(fuse_context->pid, fileINode, fi->flags, offset);
	fi->fh = fd;

	pthread_mutex_unlock(&(fileINode->iNodeMutex));
	printf("Released lock for inode %ld\n", fileINode->inode_number);
	return 0;
}
