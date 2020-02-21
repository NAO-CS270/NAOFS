#ifndef _MKDIR_H
#define _MKDIR_H

#include <sys/stat.h>

extern int createFile(const char *path, iNodeType fileType, mode_t mode);

#endif

