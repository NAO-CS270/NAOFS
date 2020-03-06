#include "dsk/mdisk.h"
#include "dsk/diskAccess.h"

#include <stdbool.h>
#include <stdio.h>

static bool isDiskAvailable = false;

int setupDisk(const char *inputDev) {
	if (isDiskAvailable) {
		printf("Uhh Ohh!! You opened the disk already. Don't be nasty!!\n");
		return -1;
	}

	int retValue = initDeviceAccessor(inputDev);
	if (retValue == 0) {
		isDiskAvailable = true;
	}
	return retValue;
}

int getDiskBlock(size_t blockNumber, disk_block *blockData) {
	if (isDiskAvailable) {
		return fetchDeviceDiskBlock(blockNumber, blockData);
	}

	return fetchMemoryDiskBlock(blockNumber, blockData);
}

int writeDiskBlock(size_t blockNumber, disk_block* blockData) {
	if (isDiskAvailable) {
		return writeDeviceDiskBlock(blockNumber, blockData);
	}

	return writeMemoryDiskBlock(blockNumber, blockData);
}

