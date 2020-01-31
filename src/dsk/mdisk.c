#include "mdisk.h"
#include <stdlib.h>

#define DISK_SIZE 1024

static disk_block mem_disk[DISK_SIZE];

disk_block* fetchMemoryDiskBlock(int blockNumber) {
	disk_block* retBlock = (disk_block*)malloc(sizeof(disk_block));
	
	if (blockNumber > DISK_SIZE) {
		// Throw appropriate exception
		*retBlock = mem_disk[0];
	}
	else {
		*retBlock = mem_disk[blockNumber - 1];
	}

	return retBlock;
}

void writeMemoryDiskBlock(int blockNumber, disk_block* blockData) {
	if (blockNumber >= DISK_SIZE) {
		// Throw appropriate exception
	}
	else {
		mem_disk[blockNumber] = *blockData;
	}
	free(blockData);
}
