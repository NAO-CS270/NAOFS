#include "free.h"
#include "mdisk.h"
#include "../mkfs/metaBlocks.h"

static pthread_mutex_t iNodeListMutex = PTHREAD_MUTEX_INITIALIZER;
const int SLEEP_TIME_IN_SECONDS = 3;

void alloc(superBlock* superBlock, size_t blockNumber) {
    while (superBlock -> free_blklst_lock)
        sleep(SLEEP_TIME_IN_SECONDS);
    pthread_mutex_lock(&iNodeListMutex);
    if (superBlock -> free_blklst_lock) {
        pthread_mutex_unlock(&iNodeListMutex);
        return alloc(superBlock);
    }

    superBlock -> free_blklst_lock = true;
    if (superBlock -> num_free_blocks == BLOCK_SIZE) {
        size_t finalBlock = superBlock -> free_block_list + superBlock -> num_free_blocks - 1;
        disk_block *new_list = (disk_block *)malloc(BLOCK_SIZE);
        fetchMemoryDiskBlock(new_block, new_list);
    } else {
        // TODO: recursively look into the last block, append it in the first place
    }

    superBlock -> dirty_bit = true;
    superBlock -> free_blklst_lock = false;
    pthread_mutex_unlock(&iNodeListMutex);
    return new_block;
}