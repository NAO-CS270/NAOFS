#ifndef _TRUNCATE_H
#define _TRUNCATE_H

#include "inode/inCoreiNode.h"

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern void truncateINode (inCoreiNode* iNode, size_t size);
extern int truncateFile (const char *path, size_t size, struct fuse_context *fuse_context);

#endif
