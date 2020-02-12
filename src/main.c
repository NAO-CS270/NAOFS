#include <sys/stat.h>
#include "main.h"
#include "trav/namei.h"
#include "utils/utils.h"
#include "mkfs/mkfs.h"
#include "incoreInodeOps/hashQ.h"
#include "mkfs/iNodeManager.h"

static int getattr_callback(const char *path, struct stat *stbuf) {
    inCoreiNode* inode = getFileINode(path, strlen(path));
    debug_print("inode %p %d %d %d %d", inode, inode -> owner_uid, inode -> group_uid, inode -> size, inode -> type);
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
	iput(inode);
    printf("returning from getAttr");
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
    printf("reading %s ", path);
    struct fuse_context* fuse_context = fuse_get_context();
    // TODO: perform access permission checks
    inCoreiNode *inode = file_descriptor_table[fi->fh].inode;
    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
    while(blockBytesRead < size) {
        bmap(inode, tempOffset, bmapResp);
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
		bmap(inode, tempOffset, bmapResp);
        if(bmapResp == NULL)
            return -1;

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
    for(i=0; i<DIRECTORY_ENTRIES_IN_BLOCK ;i++) {
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
	initHashQueues();
    // initialize the file table entries here
    initFileTableEntries();

    return fuse_main(argc, argv, &OPERATIONS, NULL);
}

