#ifndef _READ_H
#define _READ_H

#include <fuse.h>

extern int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);

#endif

