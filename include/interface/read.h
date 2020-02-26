#ifndef _READ_H
#define _READ_H

#include <fuse.h>

extern int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
extern int readFile(char *buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context);

#endif

