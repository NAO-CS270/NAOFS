#ifndef WRITE_H
#define WRITE_H

#include <fuse.h>

extern int writeToFile(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context);


#endif //WRITE_H
