#define FUSE_USE_VERSION 29

#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include "fileTables.h"
#include "inode/inCoreiNode.h"

//static const char *filepath = "/file";
//static const char *filename = "file";
//static const char *filecontent = "It might be depressing but, I am all the content here.\n";


static int getattr_callback(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (strcmp(path, filepath) == 0) {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(filecontent);
        return 0;
    }
    return -ENOENT;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, filename, NULL, 0);

    return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("Given path - %s\n", path);
    printf("Here's what I'm giving you\n");

    size_t len = strlen(filecontent);
    if (offset >= len) {
        return 0;
    }

    if (offset + size > len) {
        memcpy(buf, filecontent + offset, len - offset);
        return len - offset;
    }

    memcpy(buf, filecontent + offset, size);
    return size;
}


static int open_callback(const char *path, struct fuse_file_info *fi) {
    // TODO: Uncomment when namei is implemented
    // inCoreiNode *inode = namei(path);

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
    // TODO: Uncomment this when the new fd is allocated to this file
    return fd;
}


static struct fuse_operations OPERATIONS = {
        .getattr = getattr_callback,
        .readdir = readdir_callback,
        .read = read_callback,
        .open = open_callback,
};

int main(int argc, char *argv[]) {
    // initialize the file table entries here
    initFileTableEntries();
    return fuse_main(argc, argv, &OPERATIONS, NULL);
}