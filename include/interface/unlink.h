#ifndef UNLINK_H
#define UNLINK_H

#include "inode/inCoreiNode.h"

#include <fuse.h>
#include <stdbool.h>

extern int unlinkFile(const char* file, struct fuse_context* fuseContext);
extern int unlinkDir(const char* path, struct fuse_context* fuseContext, bool force);
extern int validateAndEmptyDirectoryIfNeeded (inCoreiNode *fileINode, bool force);
extern int unlinkFileFromINode(inCoreiNode* parentINode, char* filename);
#endif //UNLINK_H
