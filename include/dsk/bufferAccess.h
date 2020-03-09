#ifndef _BUFFER_ACCESS_H
#define _BUFFER_ACCESS_H

#include "dsk/node.h"

#include <stddef.h>

cacheNode *getBlock(size_t blockNumber, size_t deviceNumber);

void blockRelease(cacheNode *node);

#endif

