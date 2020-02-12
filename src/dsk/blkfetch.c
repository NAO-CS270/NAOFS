#include "dsk/blkfetch.h"

disk_block* getDiskBlock(size_t blockNumber, disk_block *blockData) {
	return fetchMemoryDiskBlock(blockNumber, blockData);
}

void writeDiskBlock(size_t blockNumber, disk_block* blockData) {
	writeMemoryDiskBlock(blockNumber, blockData);
}

