#ifndef _CACHE_HASH_Q_H
#define _CACHE_HASH_Q_H

#include "dsk/node.h"

#include <stddef.h>

extern void initCacheHashQueues();

extern void removeNodeFromCacheHashQ(cacheNode *node);

extern void insertNodeInCacheHashQ(cacheNode *node);

extern cacheNode* lookupBufferCache(size_t blockNumber, size_t deviceNumber);

#endif

