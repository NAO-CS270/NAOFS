#ifndef _BLOCK_FETCHER_H
#define _BLOCK_FETCHER_H

#include "mdisk.h"

extern disk_block* getDiskBlock(int blockNumber);

extern void writeDiskBlock(int blockNumber, disk_block* blockData);
#endif
