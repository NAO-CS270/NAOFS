#ifndef _GETATTR_H
#define _GETATTR_H

#include <sys/stat.h>

extern int attrPopulate(const char *path, struct stat *stbuf);
extern int changeMode(const char* path, mode_t mode);
extern int changeOwner(const char* path, uid_t uid, gid_t gid);

#endif

