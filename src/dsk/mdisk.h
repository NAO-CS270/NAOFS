#ifndef _MEMORY_DISK_H
#define _MEMORY_DISK_H

#include "mandsk/params.h"

struct disk_block {
	unsigned char data[BLOCK_SIZE];
};
typedef struct disk_block disk_block;

extern disk_block* fetchMemoryDiskBlock(int blockNumber);

extern void writeMemoryDiskBlock(int blockNumber, disk_block* diskData);

#endif
