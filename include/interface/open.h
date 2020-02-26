#ifndef _OPEN_H
#define _OPEN_H

#include "inode/inCoreiNode.h"
#include <fuse.h>

extern int openFile(const char* path, struct fuse_file_info *fi, struct fuse_context *fuse_context);
size_t calculateFileOffset (inCoreiNode *iNode, int flags);

#endif