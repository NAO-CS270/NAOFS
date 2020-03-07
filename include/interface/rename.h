#ifndef RENAME_H
#define RENAME_H

#include <fuse.h>

extern int renameFile(const char* from, const char* to, struct fuse_context* fuseContext);

#endif //RENAME_H