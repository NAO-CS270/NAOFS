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
#include "interface/link.h"
#include "interface/unlink.h"


#include <sys/stat.h>
//#include <fuse_lowlevel.h>
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
	 int retVal = createFile(path, T_REGULAR, mode);
     if (retVal != 0) {
         return retVal;
     }
     return openFile(path, fi, fuse_context);
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    return openFile(path, fi, fuse_context);
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    return readFile(buf, size, offset, fi, fuse_context);
}
static int release_callback(const char* path, struct fuse_file_info* fi) {
    struct fuse_context *fuse_context = fuse_get_context();
    closeFile(fi, fuse_context);
}

/** Change the size of a file */
static int truncate_callback(const char *path, off_t size) {
    return 0;
}

static int utimens_callback(const char *path, const struct timespec tv[2]) {
    return 0;
}

static int chown_callback(const char *path, uid_t uid, gid_t gid) {
    return 0;
}

static int chmod_callback(const char *path, mode_t mode) {
    return 0;
}

static int link_callback(const char* source, const char* target) {
    struct fuse_context *fuseContext = fuse_get_context();
    return linkFiles(source, target, fuseContext);
}

static int unlink_callback(const char* file) {
    struct fuse_context* fuseContext = fuse_get_context();
    return unlinkFile(file, fuseContext);
}

static struct fuse_operations OPERATIONS = {
	.getattr = getattr_callback,
    .mkdir = mkdir_callback,
    .readdir = readdir_callback,
	.create = create_callback,
    .open = open_callback,
    .read = read_callback,
    .truncate = truncate_callback,
    .utimens = utimens_callback,
    .chown = chown_callback,
    .chmod = chmod_callback,
    .write = write_callback,
    .unlink = unlink_callback,
    //.flush = release_callback,
    //.release = release_callback,
    //.setattr = setattr_callback,
    //.access = access_callback,
    //.link = link_callback,
};

int main(int argc, char *argv[]) {
    //TODO: create directory table for /
	makeFileSystem();
    initFreeInCoreINodeList();
	initHashQueues();

    return fuse_main(argc, argv, &OPERATIONS, NULL);
}

