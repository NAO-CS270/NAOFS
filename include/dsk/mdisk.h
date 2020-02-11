#ifndef _MEMORY_DISK_H
#define _MEMORY_DISK_H

#include "mandsk/params.h"

#include <stddef.h>

struct disk_block {
	unsigned char data[BLOCK_SIZE];
};
typedef struct disk_block disk_block;

extern disk_block* fetchMemoryDiskBlock(size_t blockNumber, disk_block* blockData);

extern void writeMemoryDiskBlock(size_t blockNumber, disk_block* blockData);

#endif

