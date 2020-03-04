#ifndef _CLOSE_H
#define _CLOSE_H

#include <fuse.h>

extern int closeFile(struct fuse_file_info *fi, struct fuse_context *fuse_context);

#endif