#include "bmap.h"

bmapResponse* bmap(inCoreiNode* inode, size_t offset) {
    size_t blockNum, blockOffset, numBytesToRead;
    blockNum = offset / BLOCK_SIZE;
    if(blockNum <= BLOCK_PTRS_PER_INODE_STRUCT) {
        blockOffset = offset % BLOCK_SIZE;
    } else {
        // indirect pointers
    }
    bmapResp = (bmapResponse*)malloc(sizeof(bmapResponse));
    bmapResp->blockNumber = blockNum;
    bmapResp->byteOffsetInBlock = blockOffset;

    numBytesToRead = BLOCK_SIZE - byteOffsetInBlock;
    bmapResp->ioBytesInBlock = numBytesToRead;
    //TODO: see if we need read ahead block number
    return bmapResp;
}
