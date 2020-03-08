#include "mkfs/diskParams.h"
#include "mkfs/metaBlocks.h"
#include "inode/iNode.h"
#include "dsk/blkfetch.h"

#include <stdlib.h>
#include <stdio.h>

void printFreeList() {
	disk_block *dataBlock = (disk_block *)malloc(sizeof(disk_block));
	getDiskBlock(FREE_LIST_BLOCK, dataBlock);

	freeDiskListBlock *theBlock = (freeDiskListBlock *)malloc(sizeof(freeDiskListBlock));
	makeFreeDiskListBlock(dataBlock, theBlock);

	printf("Super Block Free Blocks list is -\n");
	int i;
	for (i=0 ; i<BLOCK_ADDRESSES_PER_BLOCK ; i++) {
		printf("%ld ", theBlock->blkNos[i]);
	}
	printf("\n\n");

	free(dataBlock);
	free(theBlock);
}

void printFreeINodeList() {
	disk_block *dataBlock = (disk_block *)malloc(sizeof(disk_block));
	getDiskBlock(INODE_LIST_BLOCK, dataBlock);

	iNodeListBlock *theBlock = (iNodeListBlock *)malloc(sizeof(iNodeListBlock));
	makeINodeListBlock(dataBlock, theBlock);

	printf("Super Block Free iNode list is -\n");
	int i;
	for (i=0 ; i<INODE_NOS_PER_BLOCK ; i++) {
		printf("%ld ", theBlock->iNodeNos[i]);
	}
	printf("\n\n");

	free(dataBlock);
	free(theBlock);
}

void printAFreeINode(iNode *iNodePtr) {
	printf("********Printing iNode********\n");
	printf("Device Number = %ld\n", iNodePtr->device_number);
	printf("iNode Number = %ld\n", iNodePtr->inode_number);
	printf("links Count = %ld\n", iNodePtr->linksCount);
	printf("iNode Type = %d\n", iNodePtr->type);
	printf("********Done******************\n");
}

void printINodeForSanity() {
	disk_block *dataBlock = (disk_block *)malloc(sizeof(disk_block));
	getDiskBlock(INODE_BLOCKS_HEAD, dataBlock);

	iNode *listOfINodes = (iNode *)malloc(INODES_PER_BLOCK * sizeof(iNode));
	iNodesBlock *theBlock = (iNodesBlock *)malloc(sizeof(iNodesBlock));
	theBlock->iNodesList = listOfINodes;

	makeINodesBlock(dataBlock, theBlock);
	printAFreeINode(listOfINodes);

	free(dataBlock);
	free(listOfINodes);
	free(theBlock);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Pass exactly one device name\n");
		return 0;
	}

	printf("Setting up disk %s\n", argv[1]);
	int retValue = setupDisk(argv[1]);
	if (retValue == -1) {
		printf("Error in setting up Disk Access.\n");
		return 0;
	}

	printFreeList();
	printFreeINodeList();
	printINodeForSanity();

	return 0;
}

