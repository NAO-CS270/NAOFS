#include "dsk/free.h"
#include "dsk/mdisk.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

void _freeInto(size_t freeListBlockNumber, size_t blockNumber) {
    disk_block* freeListBlock = (disk_block*)malloc(sizeof(disk_block));
    freeListBlock = getDiskBlock(freeListBlockNumber, freeListBlock);
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
    diskBlock = makeFreeDiskListBlock (freeListBlock, diskBlock);

    printf("\nIN BLOCK FREE BEFORE FREE\n");
    int i = 0;
    while (i < BLOCK_ADDRESSES_PER_BLOCK) {
        printf("%ld  ", diskBlock->blkNos[i]);
        i++;
    }
    printf("\n");


    if (diskBlock -> blkNos[0]) {
        // _freeInto(diskBlock -> blkNos[BLOCK_ADDRESSES_PER_BLOCK - 1], blockNumber);
        writeDiskBlock(blockNumber, freeListBlock);
        diskBlock->blkNos[BLOCK_ADDRESSES_PER_BLOCK - 1] = blockNumber;
        int counter = 0;
        while (counter < (BLOCK_ADDRESSES_PER_BLOCK - 1)) {
            diskBlock->blkNos[counter] = 0;
            counter++;
        }
    } else {
        printf("Disk Block First entry zero\n");
        int i;
        for (i = BLOCK_ADDRESSES_PER_BLOCK - 1; i > -1; --i) {
            if (diskBlock -> blkNos[i] == 0) {
                diskBlock -> blkNos[i] = blockNumber;
                break;
            }
        }
    }

    printf("\nIN BLOCK FREE AFTER FREE\n");
    i = 0;
    while (i < BLOCK_ADDRESSES_PER_BLOCK) {
        printf("%ld  ", diskBlock->blkNos[i]);
        i++;
    }
    printf("\n");

    writeFreeDiskListBlock(diskBlock, freeListBlock);
    writeDiskBlock(freeListBlockNumber, freeListBlock);

    free(freeListBlock);
    free(diskBlock);
}

void diskBlockFree(size_t diskBlockNumber, ssize_t* leftSize, int recursionLevel) {
    if (recursionLevel) {
        disk_block *blockToBeFreed = (disk_block *) malloc(sizeof(disk_block));
        blockToBeFreed = getDiskBlock(diskBlockNumber, blockToBeFreed);
        freeDiskListBlock* diskBlocks = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
        diskBlocks = makeFreeDiskListBlock (blockToBeFreed, diskBlocks);

        int i;
        for (i = 0; i < BLOCK_ADDRESSES_PER_BLOCK && *leftSize > 0; ++i)
            diskBlockFree(diskBlocks -> blkNos[i], leftSize, recursionLevel - 1);
        free(blockToBeFreed);
        free(diskBlocks);
    }
    else
        *leftSize -= BLOCK_SIZE;

    blockFree(diskBlockNumber);
    printf("Block Freed: %ld\n", diskBlockNumber);
}

void inodeBlocksFree(inCoreiNode *inode) {
    ssize_t leftSize = inode->size;
    int i;
    for (i = 0; i <= DIRECT_BLOCK_LIMIT && leftSize > 0; ++i) {
        printf("left size: %ld\n", leftSize);
        printf("Block to be Freed: %ld\n", inode->dataBlockNums[i]);
        blockFree(inode->dataBlockNums[i]);
        inode->dataBlockNums[i] = 0;
        leftSize -= BLOCK_SIZE;
    }

    int indirectBlocks[] = {SINGLE_INDIRECT_BLOCK, DOUBLE_INDIRECT_BLOCK, TRIPLE_INDIRECT_BLOCK};
    i = 0;
    while (leftSize > 0) {
        diskBlockFree(inode->dataBlockNums[indirectBlocks[i]], &leftSize, i + 1);
        inode->dataBlockNums[indirectBlocks[i]] = 0;
        ++i;
    }
}

void blockFree(size_t blockNumber) {
    pthread_mutex_lock(&iNodeListMutex);
    _freeInto(FREE_LIST_BLOCK, blockNumber);
    pthread_mutex_unlock(&iNodeListMutex);
}
