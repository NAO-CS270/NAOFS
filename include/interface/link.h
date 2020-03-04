#ifndef LINK_H
#define LINK_H

#include <fuse.h>

extern int linkFiles(const char* source, const char* target, struct fuse_context* fuseContext);

#endif //LINK_H
