#ifndef _IALLOC_H
#define _IALLOC_H

#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"
#include "mandsk/params.h"
#include "mkfs/iNodeManager.h"

#include <pthread.h>

extern size_t getNewINode();

#endif
