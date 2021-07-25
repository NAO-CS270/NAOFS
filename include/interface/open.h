#ifndef _OPEN_H
#define _OPEN_H

#include "inode/inCoreiNode.h"

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern int openFile(const char* path, struct fuse_file_info *fi, struct fuse_context *fuse_context);
size_t calculateFileOffset (inCoreiNode *iNode, int flags);

#endif
