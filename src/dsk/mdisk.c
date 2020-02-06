#include "mdisk.h"

#define DISK_SIZE 1024

static disk_block mem_disk[DISK_SIZE];

disk_block* fetchMemoryDiskBlock(int blockNumber, disk_block *blockData) {
	if (blockNumber > DISK_SIZE) {
		// Throw appropriate exception
		*blockData = mem_disk[0];
	}
	else {
		*blockData = mem_disk[blockNumber - 1];
	}

	return blockData;
}

void writeMemoryDiskBlock(int blockNumber, disk_block* blockData) {
	if (blockNumber >= DISK_SIZE) {
		// Throw appropriate exception
	}
	else {
		mem_disk[blockNumber] = *blockData;
	}
}
