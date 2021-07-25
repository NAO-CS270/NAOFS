#ifndef WRITE_H
#define WRITE_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse3/fuse.h>

extern int writeToFile(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context);


#endif //WRITE_H
