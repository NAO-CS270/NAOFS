#ifndef UNLINK_H
#define UNLINK_H

#include <fuse.h>

extern int unlinkFile(const char* file, struct fuse_context* fuseContext);
#endif //UNLINK_H
