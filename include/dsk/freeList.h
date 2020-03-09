#ifndef _CACHE_FREE_LIST_H
#define _CACHE_FREE_LIST_H

#include "dsk/node.h"

extern void cacheFreeListInsert(cacheNode* node);

extern void initBufferCacheFreeList();

extern void cacheFreeListRemove(cacheNode* node);

extern cacheNode *getFreeBuffer();

#endif

