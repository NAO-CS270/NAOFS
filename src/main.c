#include "main.h"
#include "trav/namei.h"
#include "utils/utils.h"
#include "mkfs/mkfs.h"
#include "incoreInodeOps/hashQ.h"
#include "mkfs/iNodeManager.h"
#include "interface/getAttr.h"
#include "interface/create.h"
#include "interface/read.h"
#include "inode/iNode.h"

#include <sys/stat.h>

static int truncateFile(inCoreiNode* inode) {
    inodeBlocksFree(inode);
    inode -> size = 0;
}

static int create_callback(const char *path, mode_t mode, struct fuse_file_info *fi) {
	size_t pathLen = strlen(path);
	char *parentDirPath = (char *)malloc((pathLen + 1)*sizeof(char));
	char *filename = (char *)malloc((pathLen + 1)*sizeof(char));
    
	inCoreiNode* newFilesiNode;
    newFilesiNode = getFileINode(path, strlen(path));
    size_t fd;
    if (newFilesiNode == NULL) {
        // assign new inode from the file system
        size_t newInodeNumber = getNewINode();

        // create new directory entry in parent directory
        getParentDirectory(path, parentDirPath);
        inCoreiNode *parentInode = getFileINode(parentDirPath, strlen(parentDirPath));

        // include new file name and newly assigned inode number
        getFilenameFromPath(path, filename);
        getAndUpdateDirectoryTable(parentInode, newInodeNumber, filename);
        iput(parentInode);

        newFilesiNode = iget(newInodeNumber, 0);
        size_t fd = createFileDescriptorEntry(newFilesiNode, fi->flags);
        iput(newFilesiNode);
    } else {
        truncateFile(newFilesiNode);
    }
    fd = createFileDescriptorEntry(newFilesiNode, fi -> flags);
    iput(newFilesiNode);

	free(parentDirPath);
	free(filename);
    return fd;
}

// TODO: Update the size of the file
// TODO: Call create here
static int open_callback(const char *path, struct fuse_file_info *fi) {
    if(fi -> flags & O_CREAT) {
        return -1;//fi -> fh = create_callback(path, 0, fi);
    }

    inCoreiNode *inode;
    inode = getFileINode(path, strlen(path));
    size_t fd = createFileDescriptorEntry(inode, fi -> flags);

    if(fd == -1) {
        // print errors
        return fd;
    }

    if(fi->flags & O_TRUNC) {
        truncateFile(inode);
    }
    fi->fh = fd;
    iput(inode);
    return fd;
}

// TODO: Update the size of the file
static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

    char* dataRead;
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
    time(&inode -> access_time);
    inode -> inode_changed = true;
    iput(inode);
	free(bmapResp);
    return blockBytesRead;
}

static int write_callback(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    if(fi->fh < 0) {
        return -1;
    }
    int bytesWritten = 0;
    int tempOffset = offset;

    struct fuse_context* fuse_context = fuse_get_context();
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;

    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
    while(bytesWritten < size) {
		size_t retValue = bmap(inode, tempOffset, bmapResp, APPEND_MODE);
        if(retValue == -1) {
            return -1;
		}

        // TODO: optimize with full block write
        disk_block *metaBlock = (disk_block *) malloc(sizeof(disk_block));
        metaBlock = getDiskBlock(bmapResp -> blockNumber, metaBlock);
        unsigned char* ptrIntoBlock = metaBlock -> data + bmapResp -> byteOffsetInBlock;

        size_t bytesToWrite = min(bmapResp -> bytesLeftInBlock, size - bytesWritten);
        memcpy(ptrIntoBlock, buf, bytesToWrite);
        writeMemoryDiskBlock(bmapResp->blockNumber, metaBlock);
        free(metaBlock);
        bytesWritten += bytesToWrite;
        tempOffset += bytesToWrite;
        inode -> size = max(inode -> size, offset + bytesWritten);
    }
	free(bmapResp);
    time(&(inode -> access_time));
    time(&(inode -> modified_time));

    inode -> inode_changed = true;
    inode -> file_data_changed = true;

    iput(inode);
    return bytesWritten;
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
	return createFile(path, T_REGULAR, mode);
/*
	size_t pathLen = strlen(path);
	char *parentDirPath = (char *)malloc((pathLen + 1)*sizeof(char));
	char *filename = (char *)malloc((pathLen + 1)*sizeof(char));
    
	inCoreiNode* newFilesiNode;
    newFilesiNode = getFileINode(path, strlen(path));
    size_t fd;
    if (newFilesiNode == NULL) {
        // assign new inode from the file system
        size_t newInodeNumber = getNewINode();

        // create new directory entry in parent directory
        getParentDirectory(path, parentDirPath);
        inCoreiNode *parentInode = getFileINode(parentDirPath, strlen(parentDirPath));

        // include new file name and newly assigned inode number
        getFilenameFromPath(path, filename);
        getAndUpdateDirectoryTable(parentInode, newInodeNumber, filename);
        iput(parentInode);
    } else {
        truncateFile(newFilesiNode);
    }
    fd = createFileDescriptorEntry(newFilesiNode, fi -> flags);
    iput(newFilesiNode);

	free(parentDirPath);
	free(filename);
    return fd;
*/
}

static struct fuse_operations OPERATIONS = {
	.getattr = getattr_callback,
    .mkdir = mkdir_callback,
    .readdir = readdir_callback,
	.create = create_callback,
        //.read = read_callback,
        //.open = open_callback,
        //.write = write_callback,
        //.access = access_callback,
        //.link = link_callback,
        //.unlink = unlink_callback,
};

int main(int argc, char *argv[]) {
    //TODO: create directory table for /
	makeFileSystem();
    initFreeInCoreINodeList();
	initHashQueues();
    // initialize the file table entries here
    initFileTableEntries();

    return fuse_main(argc, argv, &OPERATIONS, NULL);
}

