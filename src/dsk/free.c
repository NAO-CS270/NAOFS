#include "dsk/free.h"
#include "dsk/mdisk.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

void _freeInto(int freeListBlockNumber, size_t blockNumber) {
    disk_block* freeListBlock = (disk_block*)malloc(sizeof(disk_block));
    freeListBlock = getDiskBlock(freeListBlockNumber, freeListBlock);
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
    diskBlock = makeFreeDiskListBlock (freeListBlock, diskBlock);

    if (diskBlock -> blkNos[0]) {
        _freeInto(diskBlock -> blkNos[BLOCK_ADDRESSES_PER_BLOCK - 1], blockNumber);
    } else {
        int i;
        for (i = BLOCK_ADDRESSES_PER_BLOCK - 1; i > -1; --i) {
            if (diskBlock -> blkNos[i] == 0) {
                diskBlock -> blkNos[i] = blockNumber;
                break;
            }
        }
    }

    free(freeListBlock);
    free(diskBlock);
}

void diskBlockFree(size_t diskBlockNumber, size_t* leftSize, int recursionLevel) {
    if (recursionLevel) {
        disk_block *blockToBeFreed = (disk_block *) malloc(sizeof(disk_block));
        blockToBeFreed = getDiskBlock(diskBlockNumber, blockToBeFreed);
        int i;
        for (i = 0; i < BLOCK_SIZE && *leftSize > 0; ++i)
            diskBlockFree(blockToBeFreed->data[i], leftSize, recursionLevel - 1);
        free(blockToBeFreed);
    }

    blockFree(diskBlockNumber);
    *leftSize -= BLOCK_SIZE;
}

void inodeBlocksFree(inCoreiNode *inode) {
    size_t leftSize = inode->size;
    int i;
    for (i = 0; i <= DIRECT_BLOCK_LIMIT && leftSize > 0; ++i) {
        blockFree(inode->dataBlockNums[i]);
        leftSize -= BLOCK_SIZE;
    }

    int indirectBlocks[] = {SINGLE_INDIRECT_BLOCK, DOUBLE_INDIRECT_BLOCK, TRIPLE_INDIRECT_BLOCK};
    i = 0;
    while (leftSize > 0) {
        diskBlockFree(inode->dataBlockNums[indirectBlocks[i]], &leftSize, i + 1);
        ++i;
    }
}

void blockFree(size_t blockNumber) {
    pthread_mutex_lock(&iNodeListMutex);
    _freeInto(FREE_LIST_BLOCK, blockNumber);
    pthread_mutex_unlock(&iNodeListMutex);
}
