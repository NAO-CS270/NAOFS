#include "string.h"
#include "dsk/alloc.h"
#include "dsk/mdisk.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

size_t blockAlloc() {
    pthread_mutex_lock(&iNodeListMutex);

    disk_block* freeListBlock = (disk_block*)malloc(sizeof(disk_block));
    freeListBlock = getDiskBlock(FREE_LIST_BLOCK, freeListBlock);
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
    diskBlock = makeFreeDiskListBlock (freeListBlock, diskBlock);

    size_t new_block = 0;
    int i;
    for (i = 0; i < BLOCK_ADDRESSES_PER_BLOCK; ++i) {
        if (diskBlock -> blkNos[i]) {
            new_block = diskBlock -> blkNos[i];
            diskBlock -> blkNos[i] = 0;
            break;
        }
    }
    if (i == BLOCK_ADDRESSES_PER_BLOCK - 1) {
        disk_block* newFreeListBlock = (disk_block*)malloc(sizeof(disk_block));
        newFreeListBlock = getDiskBlock(new_block, newFreeListBlock);
        memcpy(freeListBlock, newFreeListBlock, sizeof(disk_block));
        free(newFreeListBlock);
    }

    free(freeListBlock);
    free(diskBlock);
    pthread_mutex_unlock(&iNodeListMutex);
    return new_block;
}

