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

void diskBlockFree(disk_block* diskBlockNumber, size_t leftSize) {
    disk_block* blockToBeFreed = (disk_block*)malloc(sizeof(disk_block));
    blockToBeFreed = getDiskBlock(diskBlockNumber, blockToBeFreed);
    int i;
    for (i = 0; i < BLOCK_SIZE - 1 && leftSize > 0; ++i) {
        blockFree(blockToBeFreed -> data[i]);
        leftSize -= BLOCK_SIZE;
    }

    if (leftSize > 0) {
        diskBlockFree(blockToBeFreed->data[BLOCK_SIZE - 1], leftSize);
        blockFree(blockToBeFreed->data[BLOCK_SIZE - 1]);
    }
}

void inodeBlocksFree(inCoreiNode *inode) {
    size_t leftSize = inode->size;
    int i;
    for (i = 0; i < BLOCKS_IN_INODE - 1 && leftSize > 0; ++i) {
        blockFree(inode->dataBlockNums[i]);
        leftSize -= BLOCK_SIZE;
    }

    if (leftSize > 0) {
        diskBlockFree(inode->dataBlockNums[BLOCKS_IN_INODE - 1], leftSize);
        blockFree(inode->dataBlockNums[BLOCKS_IN_INODE - 1]);
    }
}

void blockFree(size_t blockNumber) {
    pthread_mutex_lock(&iNodeListMutex);
    _freeInto(FREE_LIST_BLOCK, blockNumber);
    pthread_mutex_unlock(&iNodeListMutex);
}
