#include "dsk/bufferAccess.h"
#include "dsk/node.h"
#include "dsk/hashQ.h"
#include "dsk/freeList.h"
#include "dsk/diskAccess.h"
#include "dsk/mdisk.h"

#include <string.h>
#include <stdlib.h>
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

		initBufferCacheFreeList();
		initCacheHashQueues();
	}
	return retValue;
}

cacheNode *makeNode(size_t blockNumber) {
    cacheNode* node = (cacheNode *)malloc(sizeof(cacheNode));
	memset(node, 0, sizeof(cacheNode));

	node->header = (blockHeader *)malloc(sizeof(blockHeader));
	memset(node->header, 0, sizeof(blockHeader));
	node->header->blockNumber = blockNumber;

    node->dataBlock = (disk_block *)malloc(sizeof(disk_block));
	memset(node->dataBlock, 0, sizeof(disk_block));
	disk_block *blockData = (disk_block *)malloc(sizeof(disk_block));

	return node;
}

cacheNode *getDiskBlockNode(size_t blockNumber, size_t deviceNumber) {
	if (isDiskAvailable) {
		return getBlock(blockNumber, deviceNumber);
	}

	cacheNode *hellyeah = makeNode(blockNumber);
	fetchMemoryDiskBlock(blockNumber, hellyeah->dataBlock);
	return hellyeah;
}

void writeDiskBlockNode(cacheNode *nodeToWrite) {
	if (isDiskAvailable) {
		blockRelease(nodeToWrite);
		return ;
	}

	writeMemoryDiskBlock(nodeToWrite->header->blockNumber, nodeToWrite->dataBlock);
}

