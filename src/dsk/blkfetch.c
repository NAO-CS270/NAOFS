#include "blkfetch.h"

disk_block* getDiskBlock(int blockNumber, disk_block *blockData) {
	return fetchMemoryDiskBlock(blockNumber, blockData);
}

void writeDiskBlock(int blockNumber, disk_block* blockData) {
	writeMemoryDiskBlock(blockNumber, blockData);
}
