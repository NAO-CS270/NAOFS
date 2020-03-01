#ifndef _CREATE_H
#define _CREATE_H

#include <sys/stat.h>
#include "inode/iNode.h"
#include "inode/inCoreiNode.h"

extern int createFile(const char *path, iNodeType fileType, mode_t mode);

#endif

