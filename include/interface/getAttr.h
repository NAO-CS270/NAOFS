#ifndef _GETATTR_H
#define _GETATTR_H

#include <fuse.h>

extern int attrPopulate(const char *path, struct stat *stbuf);

#endif
