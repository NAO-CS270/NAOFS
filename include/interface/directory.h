#ifndef _MKDIR_H
#define _MKDIR_H

#include <fuse.h>
#include <sys/stat.h>

extern int createDirectory(const char* path, mode_t mode);
extern int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);

#endif
