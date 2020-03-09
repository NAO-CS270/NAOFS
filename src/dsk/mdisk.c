#include "dsk/mdisk.h"
#include "mandsk/params.h"

#define NUM_OF_BLOCKS_IN_MEM 65536
static disk_block mem_disk[NUM_OF_BLOCKS_IN_MEM];

int fetchMemoryDiskBlock(size_t blockNumber, disk_block *blockData) {
	if (blockNumber >= NUM_OF_BLOCKS_IN_MEM) {
		return -1;
	}

	*blockData = mem_disk[blockNumber];
	return 0;
}

int writeMemoryDiskBlock(size_t blockNumber, disk_block* blockData) {
	if (blockNumber >= NUM_OF_BLOCKS_IN_MEM) {
		return -1;
	}

	mem_disk[blockNumber] = *blockData;
	return 0;
}

