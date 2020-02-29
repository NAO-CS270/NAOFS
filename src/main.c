#include "main.h"
#include "trav/namei.h"
#include "utils/utils.h"
#include "mkfs/mkfs.h"
#include "incoreInodeOps/hashQ.h"
#include "mkfs/iNodeManager.h"
#include "interface/getAttr.h"
#include "interface/create.h"
#include "interface/open.h"
#include "interface/read.h"
#include "interface/write.h"
#include "interface/close.h"
#include "inode/iNode.h"

#include <sys/stat.h>

static int truncateFile(inCoreiNode* inode) {
    inodeBlocksFree(inode);
    inode -> size = 0;
}


// TODO: Update the size of the file

static int write_callback(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    return writeToFile(path, buf, size, offset, fi, fuse_context);
}


static int access_callback(const char* path, int mode) {
    return 0;
}

static int getattr_callback(const char *path, struct stat *stbuf) {
	memset(stbuf, 0, sizeof(struct stat));

	return attrPopulate(path, stbuf);
}

static int mkdir_callback(const char* path, mode_t mode) {
	return createFile(path, T_DIRECTORY, mode);
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	return readDirectory(path, buf, filler, offset, fi);
}

static int create_callback(const char *path, mode_t mode, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
	return createFile(path, T_REGULAR, mode, fi, fuse_context);
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    return openFile(path, fi, fuse_context);
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    return readFile(buf, size, offset, fi, fuse_context);
    /*char* dataRead;
    char* ptrIntoBuf = buf;
    int blockBytesRead = 0;
    int tempOffset = offset;
    if(fi->fh < 0) {
        return -1;
    }
    printf("reading %s ", path);
    struct fuse_context* fuse_context = fuse_get_context();
    // TODO: perform access permission checks
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;
    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
    while(blockBytesRead < size) {
        bmap(inode, tempOffset, bmapResp, READ_MODE);
        // trying to read end of the file
        if(bmapResp->bytesLeftInBlock == 0) {
            break;
        }
        // fetch the block from the disk and copy the data in the buffer
        disk_block* metaBlock = (disk_block*)malloc(sizeof(disk_block));
        metaBlock = getDiskBlock(bmapResp -> blockNumber, metaBlock);
        size_t bytesToRead = min(bmapResp -> bytesLeftInBlock, size - blockBytesRead);
        memcpy(ptrIntoBuf, metaBlock -> data + bmapResp -> byteOffsetInBlock, bytesToRead);
        ptrIntoBuf += bytesToRead;

        blockBytesRead += bytesToRead;
        tempOffset = tempOffset + bytesToRead;
        free(metaBlock);
    }
    inode -> size = max(inode -> size, size);
    time(&inode -> access);
    inode -> inode_changed = true;
    iput(inode);
	free(bmapResp);
    return blockBytesRead;*/
}
static int release_callback(const char* path, struct fuse_file_info* fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    closeFile(fi, fuse_context);
}

static struct fuse_operations OPERATIONS = {
	.getattr = getattr_callback,
    .mkdir = mkdir_callback,
    .readdir = readdir_callback,
	.create = create_callback,
    .open = open_callback,
    .read = read_callback,
    .flush = release_callback,
    //.release = release_callback,
    .write = write_callback,
        //.access = access_callback,
        //.link = link_callback,
        //.unlink = unlink_callback,
};

int main(int argc, char *argv[]) {
    //TODO: create directory table for /
	makeFileSystem();
    initFreeInCoreINodeList();
	initHashQueues();

    return fuse_main(argc, argv, &OPERATIONS, NULL);
}

