#define FUSE_USE_VERSION 29

#include "main.h"


static int getattr_callback(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

//    if (strcmp(path, filepath) == 0) {
//        stbuf->st_mode = S_IFREG | 0777;
//        stbuf->st_nlink = 1;
//        //stbuf->st_size = strlen(filecontent);
//        return 0;
//    }
    return -ENOENT;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    inCoreiNode *inode;
    // TODO: Uncomment when namei is implemented
    // inode = namei(path);

    // TODO: Allocate file table entry for inode, initialize count, offset.
    size_t fd = putFileDescriptorEntry(inode, fi->flags);
    if(fd == -1) {
        return fd;
    }
    // TODO: Allocate user file descriptor entry, set pointer to file table entry - no need to do this.
    if(fi->flags & O_TRUNC) {
        // TODO: Free all blocks. (Algorithm: free)
    }
    inode->lock = false;
    // set the file handle in file_info
    fi->fh = fd;
    return fd;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

    char* dataRead;
    char* ptrIntoBuf = buf;
    int blockBytesRead = 0;
    int tempOffset = offset;
    char* ptrIntoBuffer = buf;
    if(fi->fh < 0) {
        return -1;
    }
    struct fuse_context* fuse_context = fuse_get_context();
    // TODO: perform access permission checks
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;
    inode->lock = true;
    while(blockBytesRead < size) {
        bmapResponse *bmapResp  = bmap(inode, tempOffset);
        // trying to read end of the file
        if(bmapResp->ioBytesInBlock == 0) {
            break;
        }
        // fetch the block from the disk and copy the data in the buffer
        disk_block* metaBlock = (disk_block*)malloc(sizeof(disk_block));
        metaBlock = getDiskBlock(bmapResp->blockNumber, metaBlock);
        memcpy(ptrIntoBuf, metaBlock->data, bmapResp->ioBytesInBlock);
        ptrIntoBuf += bmapResp->ioBytesInBlock + 1;

        blockBytesRead += bmapResp->ioBytesInBlock;
        tempOffset = tempOffset + blockBytesRead + 1;
        free(metaBlock);
    }
    inode->lock = false;
    return blockBytesRead;
}

static int write_callback(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    if(fi->fh < 0) {
        return -1;
    }
    int bytesWritten = 0;
    bool fullBlockWrite = false;
    int tempOffset = offset;
    char* ptrIntoBuffer = buf;

    struct fuse_context* fuse_context = fuse_get_context();
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;

    inode->lock = true;
    while(bytesWritten < size) {
        bmapResponse *bmapResp = bmap(inode, tempOffset);
        if(bmapResp == NULL) {
            // TODO: Call alloc here
            fullBlockWrite = true;
        } else {
            if (bmapResp->byteOffsetInBlock == 0 && bmapResp->ioBytesInBlock == BLOCK_SIZE) {
                // no need to read the block from the disk. This is cause the entire block has to
                // be overwritten anyway.
                fullBlockWrite = true;
            }
        }
        disk_block *metaBlock = (disk_block *) malloc(sizeof(disk_block));
        if(fullBlockWrite) {
            memcpy(metaBlock->data, buf, bmapResp->ioBytesInBlock);
            writeMemoryDiskBlock(bmapResp->blockNumber, metaBlock);
        } else {
            metaBlock = fetchMemoryDiskBlock(bmapResp->blockNumber, metaBlock);
            unsigned char* ptrIntoBlock = metaBlock->data;
            ptrIntoBlock += bmapResp->ioBytesInBlock;
            memcpy(ptrIntoBlock, buf, bmapResp->ioBytesInBlock);
            writeMemoryDiskBlock(bmapResp->blockNumber, metaBlock);
        }
        free(metaBlock);
        bytesWritten += bytesWritten + bmapResp->ioBytesInBlock;
        tempOffset = tempOffset + bmapResp->ioBytesInBlock + 1;
        }
    }
    return bytesWritten;
}

static int mkdir_callback(const char* path, mode_t mode) {
    // if new node  not named pipe and user not super user
    // return error
    // get inode of parent node (using namei)

}

static struct fuse_operations OPERATIONS = {
        .getattr = getattr_callback,
        .read = read_callback,
        .open = open_callback,
        .write = write_callback
        //.mkdir = mkdir_callback,
};

int main(int argc, char *argv[]) {
    // initialize the file table entries here
    initFileTableEntries();
    return fuse_main(argc, argv, &OPERATIONS, NULL);
}