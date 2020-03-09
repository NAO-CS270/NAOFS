#include "dsk/node.h"
#include "dsk/mdisk.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

size_t blockAlloc() {
    pthread_mutex_lock(&iNodeListMutex);

	cacheNode *freeListBlockNode = getDiskBlockNode(FREE_LIST_BLOCK, 0);
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
    makeFreeDiskListBlock(freeListBlockNode->dataBlock, diskBlock);

    printf("\nIN BLOCK ALLOC BEFORE ALLOC\n");
    int i = 0;
    while (i < BLOCK_ADDRESSES_PER_BLOCK) {
        printf("%ld  ", diskBlock->blkNos[i]);
        i++;
    }
    printf("\n");

    size_t new_block = 0;
    size_t counter;
    for (counter = 0; counter < BLOCK_ADDRESSES_PER_BLOCK ; counter++) {
        if (diskBlock -> blkNos[counter] != 0) {
            new_block = diskBlock -> blkNos[counter];
            diskBlock -> blkNos[counter] = 0;
            break;
        }
    }
    if (counter == (BLOCK_ADDRESSES_PER_BLOCK - 1)) {
		cacheNode *newBlockNode = getDiskBlockNode(new_block, 0);
		memcpy(freeListBlockNode->dataBlock, newBlockNode->dataBlock, BLOCK_SIZE);
		writeDiskBlockNode(newBlockNode);
    }
	else {
		writeFreeDiskListBlock(diskBlock, freeListBlockNode->dataBlock);
	}
	freeListBlockNode->header->delayedWrite = true;
	writeDiskBlockNode(freeListBlockNode);

    free(diskBlock);
    pthread_mutex_unlock(&iNodeListMutex);
    return new_block;
}

