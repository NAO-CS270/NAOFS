#include "blkfetch.h"

disk_block* getDiskBlock(int blockNumber) {
	return fetchMemoryDiskBlock(blockNumber);
}

void writeDiskBlock(int blockNumber, disk_block* blockData) {
	writeMemoryDiskBlock(blockNumber, blockData);
}
