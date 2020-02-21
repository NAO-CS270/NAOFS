#ifndef _BLOCK_FETCHER_H
#define _BLOCK_FETCHER_H

#include "dsk/mdisk.h"

extern disk_block* getDiskBlock(size_t blockNumber, disk_block *blockData);

extern void writeDiskBlock(size_t blockNumber, disk_block* blockData);

#endif

