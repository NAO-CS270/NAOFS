#ifndef RENAME_H
#define RENAME_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern int renameFile(const char* from, const char* to, struct fuse_context* fuseContext);

#endif //RENAME_H
