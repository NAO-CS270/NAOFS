#include "alloc.h"
#include "mdisk.h"
#include "../mkfs/metaBlocks.h"

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;
const int SLEEP_TIME_IN_SECONDS = 3;

size_t alloc(superBlock* superBlock) {
    while (superBlock -> free_blklst_lock)
        sleep(SLEEP_TIME_IN_SECONDS);
    pthread_mutex_lock(&iNodeListMutex);
    if (superBlock -> free_blklst_lock) {
        pthread_mutex_unlock(&iNodeListMutex);
        return alloc(superBlock);
    }

    superBlock -> free_blklst_lock = true;
    superBlock -> num_free_blocks --;
    size_t new_block = *(superBlock -> free_block_list);
    superBlock -> free_block_list ++;

    if (superBlock -> num_free_blocks == 0) {
        superBlock -> free_block_list -= BLOCK_SIZE // is it correct?
        superBlock -> num_free_blocks = BLOCK_SIZE;
        disk_block *new_list = (disk_block *)malloc(BLOCK_SIZE);
        fetchMemoryDiskBlock(new_block, new_list);
        memcpy(superBlock -> free_block_list, new_list, BLOCK_SIZE);
    }

    superBlock -> dirty_bit = true;
    superBlock -> free_blklst_lock = false;
    pthread_mutex_unlock(&iNodeListMutex);
    return new_block;
}