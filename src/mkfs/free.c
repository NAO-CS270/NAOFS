#include "dsk/mdisk.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;

void _freeInto(size_t freeListBlockNumber, size_t blockNumber) {
	cacheNode *freeListBlockNode = getDiskBlockNode(freeListBlockNumber, 0);
    freeDiskListBlock* diskBlock = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
    diskBlock = makeFreeDiskListBlock(freeListBlockNode->dataBlock, diskBlock);

    printf("\nIN BLOCK FREE BEFORE FREE\n");
    int i = 0;
    while (i < BLOCK_ADDRESSES_PER_BLOCK) {
        printf("%ld  ", diskBlock->blkNos[i]);
        i++;
    }
    printf("\n");

    if (diskBlock -> blkNos[0]) {
		cacheNode *freeIntoBlockNode = getDiskBlockNode(blockNumber, 0);
		memcpy(freeIntoBlockNode->dataBlock, freeListBlockNode->dataBlock, BLOCK_SIZE);
		freeIntoBlockNode->header->delayedWrite = true;
		writeDiskBlockNode(freeIntoBlockNode);

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

    writeFreeDiskListBlock(diskBlock, freeListBlockNode->dataBlock);
	freeListBlockNode->header->delayedWrite = true;
    writeDiskBlockNode(freeListBlockNode);

    free(diskBlock);
}

void blockFree(size_t blockNumber) {
    pthread_mutex_lock(&iNodeListMutex);
    _freeInto(FREE_LIST_BLOCK, blockNumber);
    pthread_mutex_unlock(&iNodeListMutex);
}


void diskBlockFree(size_t diskBlockNumber, ssize_t* leftSize, int recursionLevel) {
    if (recursionLevel) {
		cacheNode *toBeFreedBlockNode = getDiskBlockNode(diskBlockNumber, 0);
        freeDiskListBlock* diskBlocks = (freeDiskListBlock*)malloc(sizeof(freeDiskListBlock));
        diskBlocks = makeFreeDiskListBlock(toBeFreedBlockNode->dataBlock, diskBlocks);

        int i;
        for (i = 0; i < BLOCK_ADDRESSES_PER_BLOCK && *leftSize > 0; ++i)
            diskBlockFree(diskBlocks -> blkNos[i], leftSize, recursionLevel - 1);
		writeDiskBlockNode(toBeFreedBlockNode);
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

