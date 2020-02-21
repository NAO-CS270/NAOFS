#include "dsk/blkfetch.h"

#include <stdio.h>

disk_block* getDiskBlock(size_t blockNumber, disk_block *blockData) {
	//printf("Got a read request for block number %ld\n", blockNumber);
	return fetchMemoryDiskBlock(blockNumber, blockData);
}

void writeDiskBlock(size_t blockNumber, disk_block* blockData) {
	//printf("Got a write request for block number %ld\n", blockNumber);
	writeMemoryDiskBlock(blockNumber, blockData);
}

