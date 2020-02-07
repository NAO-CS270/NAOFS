#include "blkfetch.h"

disk_block* getDiskBlock(int blockNumber) {
	return fetchMemoryDiskBlock(blockNumber);
}

void writeDiskBlock(int blockNumber, disk_block* blockData) {
	writeMemoryDiskBlock(blockNumber, blockData);
}

char* readDiskBlock(int blockNumber, size_t numBytesToRead) {
    disk_block* metaBlock = getDiskBlock(blockNumber);
    char *buffer;
    memcpy(buffer, metaBlock->data, numBytesToRead);
    return buffer;
}
