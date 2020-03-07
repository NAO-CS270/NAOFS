#ifndef _TRUNCATE_H
#define _TRUNCATE_H

#include "inode/inCoreiNode.h"

#include <fuse.h>

extern void truncateINode (inCoreiNode* iNode, size_t size);
extern int truncateFile (const char *path, size_t size, struct fuse_context *fuse_context);

#endif
