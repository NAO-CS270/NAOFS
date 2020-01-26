#include "mdisk.h"

#define BLOCK_SIZE 1024
#define DISK_SIZE 1024

struct disk_block {
	unsigned char data[BLOCK_SIZE];
};

static struct disk_block mem_disk[DISK_SIZE];

struct disk_block fetchMemoryDiskBlock(int blockNumber) {
	if (blockNumber > DISK_SIZE) {
		// Throw appropriate exception
		return mem_disk[0];
	}

	return mem_disk[blockNumber - 1];
}
