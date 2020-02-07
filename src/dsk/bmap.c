#include "bmap.h"

bmapResponse* bmap(inCoreiNode* inode, size_t offset) {
    size_t blockNum, numBytesToRead;
    int numBlkIndirectBlk = BLOCK_SIZE / BLOCK_ADDRESS_SIZE;
    inodeBlockIndex = offset / BLOCK_SIZE;
    if(inodeBlockIndex < DIRECT_BLOCKS) {
        blockNum = inode->disk_iNode->direct_blocks[inodeBlockIndex];

    } else if(inodeBlockIndex < numBlkIndirectBlk) {
        // get the block number in the indirect block table
        disk_block* diskBlock = fetchMemoryDiskBlock(inode->disk_iNode->single_indirect_blocks[0], diskBlock);
        indirectBlock* indirectBlock = (indirectBlock*)malloc(sizeof(indirectBlock));
        memcpy(indirectBlock, diskBlock, sizeof(indirectBlock));
        size_t newOffset = offset - BLOCK_SIZE * DIRECT_BLOCKS;
        size_t index = newOffset / BLOCK_SIZE;
        blockNum = indirectBlock->blocks[index];
    }
    bmapResponse *bmapResp = (bmapResponse*)malloc(sizeof(bmapResponse));
    bmapResp->blockNumber = blockNum;
    bmapResp->byteOffsetInBlock = offset % BLOCK_SIZE;;

    numBytesToRead = BLOCK_SIZE - bmapResp->byteOffsetInBlock;
    bmapResp->ioBytesInBlock = numBytesToRead;
    //TODO: see if we need read ahead block number
    return bmapResp;
}
