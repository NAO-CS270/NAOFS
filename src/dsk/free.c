#include "free.h"
#include "mdisk.h"
#include "../mkfs/metaBlocks.h"
#include "../mkfs/diskParams.h"
#include "blkfetch.h"

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

void free(size_t blockNumber) {
    pthread_mutex_lock(&iNodeListMutex);
    _freeInto(FREE_LIST_BLOCK, blockNumber);
    pthread_mutex_unlock(&iNodeListMutex);
}

void _freeInto(int freeListBlockNumber, size_t blockNumber) {
    disk_block* freeListBlock = (disk_block*)malloc(sizeof(disk_block));
    freeListBlock = getDiskBlock(freeListBlockNumber, freeListBlock);
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
    diskBlock = makeFreeDiskListBlock (freeListBlock, diskBlock);

    if (diskBlock -> blkNos[BLOCK_ADDRESSES_PER_BLOCK - 1]) {
        _freeInto(diskBlock -> blkNos[BLOCK_ADDRESSES_PER_BLOCK - 1], blockNumber);
    } else {
        for (int i = 0; i < BLOCK_ADDRESSES_PER_BLOCK; ++i) {
            if (diskBlock -> blkNos[i] == 0) {
                diskBlock -> blkNos[i] = blockNumber;
                break;
            }
        }
    }

    free(freeListBlock);
    free(diskBlock);
}