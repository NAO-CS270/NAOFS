#ifndef LINK_H
#define LINK_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern int linkFiles(const char* source, const char* target, struct fuse_context* fuseContext);

#endif //LINK_H
