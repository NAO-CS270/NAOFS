#include "mkfs/mkfs.h"
#include "dsk/blkfetch.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Pass exactly one device name\n");
		return 0;
	}

	printf("Setting up disk %s\n", argv[1]);
	int retValue = setupDisk(argv[1]);
	if (retValue == -1) {
		printf("Error in setting up Disk Access.\n");
		return -1;
	}

	printf("Making filesystem\n");
	makeFileSystem();

	return 0;
}

