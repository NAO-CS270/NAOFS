#ifndef _IALLOC_H
#define _IALLOC_H

#include <sys/types.h>

extern size_t getNewINode(iNodeType fileType, mode_t fileMode, uid_t uid, gid_t gid);

#endif

