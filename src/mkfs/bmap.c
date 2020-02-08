#include "bmap.h"

const size_t blkAddrNos = BLOCK_SIZE/BLOCK_ADDRESS_SIZE;

bmapResponse* bmap(inCoreiNode* inode, size_t offset) {
    bmapResponse* bmapResp = (bmapResponse*)malloc(sizeof(bmapResponse));
    bmapResp->byteOffsetInBlock = offset % BLOCK_SIZE;
    size_t numBytesToRead = BLOCK_SIZE - bmapResp->byteOffsetInBlock;
    bmapResp->ioBytesInBlock = numBytesToRead;

    size_t blockNum;
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
    bmapResp->blockNumber = blockNum;
    //TODO: see if we need read ahead block number
    return bmapResp;
}
