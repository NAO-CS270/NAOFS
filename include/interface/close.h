#ifndef _CLOSE_H
#define _CLOSE_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern int closeFile(struct fuse_file_info *fi, struct fuse_context *fuse_context);

#endif
