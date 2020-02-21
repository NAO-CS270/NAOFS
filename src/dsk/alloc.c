#include "dsk/alloc.h"
#include "dsk/mdisk.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

#include <stdlib.h>

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

size_t blockAlloc() {
    pthread_mutex_lock(&iNodeListMutex);

    disk_block* freeListBlock = (disk_block*)malloc(sizeof(disk_block));
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));

    getDiskBlock(FREE_LIST_BLOCK, freeListBlock);
    makeFreeDiskListBlock (freeListBlock, diskBlock);

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
		getDiskBlock(new_block, freeListBlock);
    }
	else {
		writeFreeDiskListBlock(diskBlock, freeListBlock);
	}
	writeDiskBlock(FREE_LIST_BLOCK, freeListBlock);

	free(freeListBlock);
    free(diskBlock);
    pthread_mutex_unlock(&iNodeListMutex);
    return new_block;
}

