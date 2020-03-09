#ifndef _CACHE_NODE_H
#define _CACHE_NODE_H

#include "dsk/mdisk.h"

#include <stdbool.h>
#include <pthread.h>

struct blockHeader {
	size_t deviceNumber;
	size_t blockNumber;
	bool delayedWrite;
	
    pthread_mutex_t bufferLock;
};
typedef struct blockHeader blockHeader;

struct cacheNode {
	blockHeader* header;
    disk_block* dataBlock;

    struct cacheNode* prev;
    struct cacheNode* next;

    struct cacheNode* hash_prev;
    struct cacheNode* hash_next;
};
typedef struct cacheNode cacheNode;

#endif

