#include "dsk/mdisk.h"
#include "mandsk/params.h"

static disk_block mem_disk[DISK_SIZE];

disk_block* fetchMemoryDiskBlock(size_t blockNumber, disk_block *blockData) {
	if (blockNumber > DISK_SIZE) {
		// Throw appropriate exception
		*blockData = mem_disk[0];
	}
	else {
		*blockData = mem_disk[blockNumber];
	}

	return blockData;
}

void writeMemoryDiskBlock(size_t blockNumber, disk_block* blockData) {
	if (blockNumber >= DISK_SIZE) {
		// Throw appropriate exception
	}
	else {
		mem_disk[blockNumber] = *blockData;
	}
}

