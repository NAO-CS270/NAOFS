#include "blkfetch.h"

disk_block* getDiskBlock(int blockNumber) {
	return fetchMemoryDiskBlock(blockNumber);
}
