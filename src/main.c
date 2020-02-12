#include <sys/stat.h>
#include "main.h"
#include "trav/namei.h"
#include "utils/utils.h"
#include "mkfs/mkfs.h"

static int getattr_callback(const char *path, struct stat *stbuf) {
    inCoreiNode* inode = getFileINode(path, strlen(path));
    if (inode == NULL)
        return -ENOENT;

    memset(stbuf, 0, sizeof(struct stat));
    stbuf -> st_atime = inode -> access_time;
    stbuf -> st_mtime = inode -> modified_time;
    stbuf -> st_ctime = inode -> modified_time;
    stbuf -> st_uid = (uid_t) inode -> owner_uid;
    stbuf -> st_gid = (gid_t) inode -> group_uid;
//    stbuf -> st_nlink = inode -> linksCount;  TODO
    stbuf -> st_size = inode -> size;

    if (inode -> type == T_DIRECTORY) {
        stbuf -> st_mode = S_IFDIR | 0755;
        stbuf -> st_nlink = 2;
    } else {
        stbuf -> st_mode = S_IFREG | 0777;
        stbuf -> st_nlink = 1;
    }
    return 0;
}

static int truncateFile(inCoreiNode* inode) {
    inodeBlocksFree(inode);
    inode -> size = 0;
}

static int create_callback(const char *path, mode_t mode, struct fuse_file_info *fi) {
    inCoreiNode* newFilesiNode;
    newFilesiNode = getFileINode(path, strlen(path));
    size_t fd;
    if (newFilesiNode == NULL) {
        // assign new inode from the file system
        size_t newInodeNumber = getNewINode();

        // create new directory entry in parent directory
        char *parentDirPath;
        parentDirPath = getParentDirectory(path);
        inCoreiNode *parentInode = getFileINode(parentDirPath, strlen(parentDirPath));

        // include new file name and newly assigned inode number
        char *filename = getFilenameFromPath(path);
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
    return fd;
}

// TODO: Update the size of the file
// TODO: Call create here
static int open_callback(const char *path, struct fuse_file_info *fi) {
    if(fi -> flags & O_CREAT) {
        return fi -> fh = create_callback(path, 0, fi);
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
    struct fuse_context* fuse_context = fuse_get_context();
    // TODO: perform access permission checks
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;
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
    fetchInodeFromDisk(inode->inode_number, inode);
    inode->size = size;
    iput(inode);
    return blockBytesRead;
}

static int write_callback(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    if(fi->fh < 0) {
        return -1;
    }
    int bytesWritten = 0;
    bool fullBlockWrite = false;
    int tempOffset = offset;

    struct fuse_context* fuse_context = fuse_get_context();
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;

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
            metaBlock = getDiskBlock(bmapResp->blockNumber, metaBlock);
            unsigned char* ptrIntoBlock = metaBlock->data;
            ptrIntoBlock += bmapResp->ioBytesInBlock;
            memcpy(ptrIntoBlock, buf, bmapResp->ioBytesInBlock);
            writeMemoryDiskBlock(bmapResp->blockNumber, metaBlock);
        }
        free(metaBlock);
        bytesWritten += bytesWritten + bmapResp->ioBytesInBlock;
        tempOffset = tempOffset + bmapResp->ioBytesInBlock + 1;
    }
    fetchInodeFromDisk(inode->inode_number, inode);
    inode->size = bytesWritten;
    iput(inode);
    return bytesWritten;
}

/* 
 * Edge cases to handle:
 * Handle the directory to create is `..`.
 * creates a new special file(dir, pipe, link). Returns -1 on error
 */
static int mkdir_callback(const char* path, mode_t mode) {
    char *parentDirPath = getParentDirectory(path);
	inCoreiNode *parentINode = getFileINode(parentDirPath, strlen(parentDirPath));
	if (NULL == parentINode) {
		return -1;
	}

    char *filename = getFilenameFromPath(path);
	size_t iNodeNum = findINodeInDirectory(parentINode, filename);
	if (iNodeNum != 0) {
		return -1;
	}

	iNodeNum = getNewINode();
    getAndUpdateDirectoryTable(parentINode, iNodeNum, filename);

    // TODO: use the right device number, using 0 for now
    inCoreiNode *newINode = iget(iNodeNum, 0);
    newINode -> type = T_DIRECTORY;
    // size_t fd = createFileDescriptorEntry(newFilesiNode, mode);;
    // add "." and ".." in the newly created inode
    updateNewDirMetaData(newINode, iNodeNum, parentINode->inode_number);

	iput(parentINode);
    iput(newINode);
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
//    filler(buf, ".", NULL, 0);
//    filler(buf, "..", NULL, 0);

    inCoreiNode* inode = getFileINode(path, strlen(path));
    if(inode == NULL) {
        debug_print("inode not found for path: %s", path);
        return 0;
    }
    directoryTable* dirTable = getDirectoryEntries(inode);

    int i;
    nameINodePair *iNodeData;
    for(i=0; i<ENTRIES_PER_BLOCK;i++) {
        iNodeData = &(dirTable->entries[i]);
        filler(buf, iNodeData->name, NULL, 0);
    }
    return 0;
}

static int access_callback(const char* path, int mode) {
    return 0;
}

static struct fuse_operations OPERATIONS = {
        .getattr = getattr_callback,
        .read = read_callback,
        .open = open_callback,
        .write = write_callback,
        .mkdir = mkdir_callback,
        .create = create_callback,
        .readdir = readdir_callback,
        .access = access_callback,
//        .link = link_callback,
//        .unlink = unlink_callback,
};

int main(int argc, char *argv[]) {
    //TODO: create directory table for /
	makeFileSystem();
    initFreeInCoreINodeList();

    // initialize the file table entries here
    initFileTableEntries();
    return fuse_main(argc, argv, &OPERATIONS, NULL);
}

