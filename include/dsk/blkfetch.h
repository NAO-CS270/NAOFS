#ifndef _BLOCK_FETCHER_H
#define _BLOCK_FETCHER_H

#include "dsk/mdisk.h"

extern int setupDisk(const char *inputDev);

extern int getDiskBlock(size_t blockNumber, disk_block *blockData);

extern int writeDiskBlock(size_t blockNumber, disk_block* blockData);

#endif

