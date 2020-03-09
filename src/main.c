#include "main.h"
#include "trav/namei.h"
#include "utils/utils.h"
#include "incoreInodeOps/hashQ.h"
#include "mkfs/iNodeManager.h"
#include "interface/getAttr.h"
#include "interface/create.h"
#include "interface/open.h"
#include "interface/read.h"
#include "interface/write.h"
#include "interface/close.h"
#include "interface/rename.h"
#include "interface/truncate.h"
#include "inode/iNode.h"
#include "interface/link.h"
#include "interface/unlink.h"
#include "fdTable/globalFileTable.h"
#include "mandsk/params.h"
#include "mkfs/mkfs.h"
#include "mandsk/params.h"

#include <sys/stat.h>
//#include <fuse_lowlevel.h>
// static int truncateFile(inCoreiNode* inode) {
//     inodeBlocksFree(inode);
//     inode -> size = 0;
// }

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
    struct fuse_context *fuse_context = fuse_get_context();
	return createFile(path, T_DIRECTORY, mode, fuse_context);
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	return readDirectory(path, buf, filler, offset, fi);
}

static int create_callback(const char *path, mode_t mode, struct fuse_file_info *fi) {
    struct fuse_context *fuse_context = fuse_get_context();
	 int retVal = createFile(path, T_REGULAR, mode, fuse_context);
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
    struct fuse_context *fuse_context = fuse_get_context();
    return truncateFile (path, size, fuse_context);
}

static int utimens_callback(const char *path, const struct timespec tv[2]) {
    return 0;
}

static int chown_callback(const char *path, uid_t uid, gid_t gid, struct fuse_file_info* fi) {
    changeOwner(path, uid, gid);
    return 0;
}

static int chmod_callback(const char *path, mode_t mode) {
    changeMode(path, mode);
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

static int unlink_dir_callback(const char* file) {
    struct fuse_context* fuseContext = fuse_get_context();
    return unlinkDir(file, fuseContext, false);
}

static int rename_callback (const char* from, const char* to) {
    struct fuse_context* fuseContext = fuse_get_context();
    return renameFile(from, to, fuseContext);
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
    .release = release_callback,
    .rename = rename_callback,
    .rmdir = unlink_dir_callback,
    //.setattr = setattr_callback,
    //.access = access_callback,
    //.link = link_callback,
};

int parseCmdArg(int argc, char *argv[], char *buf) {
	int i;
	char *patPtr;
	for (i=0 ; i<argc ; i++) {
		patPtr = strstr(argv[i], "fsname");
		if (patPtr == argv[i]) {
			patPtr = patPtr + 7;
			strcpy(buf, patPtr);
			return 1;
		}
	}
	return 0;
}

int checkDevMode(int argc, char *argv[]) {
	char *devName = (char *)malloc(20 * sizeof(char));
	int retValue;
	if (parseCmdArg(argc, argv, devName) == 1) {
		printf("Setting up disk %s\n", devName);
		if (setupDisk(devName) == -1) {
			retValue = -1;
		}
		else {
			retValue = 0;
		}
	}
	else {
		makeFileSystem();
		retValue = 0;
	}
	free(devName);
	return retValue;
}

int main(int argc, char *argv[]) {
	if (checkDevMode(argc, argv) == -1) {
		printf("Error in setting up Disk\n");
		return 0;
	}

	initializeINodeParams();
	initFreeInCoreINodeList();
	initHashQueues();
    initGlobalFileTable();

    return fuse_main(argc, argv, &OPERATIONS, NULL);
}

