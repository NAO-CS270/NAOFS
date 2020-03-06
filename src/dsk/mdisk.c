#include "dsk/mdisk.h"
#include "mandsk/params.h"

static disk_block mem_disk[NUM_OF_BLOCKS];

int fetchMemoryDiskBlock(size_t blockNumber, disk_block *blockData) {
	if (blockNumber >= NUM_OF_BLOCKS) {
		return -1;
	}

	*blockData = mem_disk[blockNumber];
	return 0;
}

int writeMemoryDiskBlock(size_t blockNumber, disk_block* blockData) {
	if (blockNumber >= NUM_OF_BLOCKS) {
		return -1;
	}

	mem_disk[blockNumber] = *blockData;
	return 0;
}

