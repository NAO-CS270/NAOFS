#ifndef _BLOCK_FETCHER_H
#define _BLOCK_FETCHER_H

#include "dsk/mdisk.h"
#include "dsk/node.h"

extern int setupDisk(const char *inputDev);

extern cacheNode *getDiskBlockNode(size_t blockNumber, size_t deviceNumber);

extern void writeDiskBlockNode(cacheNode *nodeToWrite);

#endif

