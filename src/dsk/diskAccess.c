#include "mandsk/params.h"
#include "dsk/mdisk.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static char *devName = NULL;

static int devFD;

int initDeviceAccessor(const char *inputDev) {
	if (devName != NULL) {
		printf("Not Again!!\n");
		return -1;
	}
	if (access(inputDev, R_OK | W_OK) != 0) {
		printf("Device file either doesn't exist or you don't have permissions to it.\n");
		return -1;
	}

	devName = strdup(inputDev);
	devFD = open(devName, O_RDWR);
	return 0;
}

int fetchDeviceDiskBlock(size_t blockNumber, disk_block *blockData) {
	if (blockNumber >= NUM_OF_BLOCKS) {
		printf("Isn't the disk enough for you?? Get back in range!!\n");
		return -1;
	}

	lseek(devFD, blockNumber*BLOCK_SIZE, SEEK_SET);
	if (read(devFD, blockData->data, BLOCK_SIZE) != BLOCK_SIZE) {
		printf("Couldn't read the block.\n");
		return -1;
	}

	return 0;
}

int writeDeviceDiskBlock(size_t blockNumber, disk_block* blockData) {
	if (blockNumber >= NUM_OF_BLOCKS) {
		printf("Isn't the disk enough for you?? Get back in range!!\n");
		return -1;
	}

	lseek(devFD, blockNumber*BLOCK_SIZE, SEEK_SET);
	if (write(devFD, blockData->data, BLOCK_SIZE) == -1) {
		printf("Couldn't write the block.\n");
		return -1;
	}

	return 0;
}

