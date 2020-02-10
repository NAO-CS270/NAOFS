#include "dsk/blkfetch.h"
#include "mkfs/mkfs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void testBlockFetch() {
	disk_block *theBlock = (disk_block *)malloc(sizeof(disk_block));
	disk_block *testBlock = (disk_block *)malloc(sizeof(disk_block));

	char temp[10] = "hellyeah";
	memcpy(theBlock->data, temp, 10);
	writeDiskBlock(43, theBlock);
	getDiskBlock(43, testBlock);

	if (strcmp(temp, testBlock->data)) {
		printf("Block Fetch is working !!\n");
	}
	else {
		printf("Failed\n");
	}
}

void testMKFS() {
	makeFileSystem();
	printf("MKFS runs. Need not be correctly.\n");
}

int main() {
	testBlockFetch();
	testMKFS();

	return 0;
}
