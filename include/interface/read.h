#ifndef _READ_H
#define _READ_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
extern int readFile(char *buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context);

#endif

