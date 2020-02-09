#include "./bmap.h"

// num of block addresses in indirect block
const size_t blkAddrNos = BLOCK_SIZE / BLOCK_ADDRESS_SIZE;
// max size mapped by all the direct blocks
const size_t directBlkMaxSize = BLOCK_SIZE * DIRECT_BLOCKS;
// max size mapped by all the single indirect blocks
const size_t singleIndirectBlkMaxSize = BLOCK_SIZE * blkAddrNos;
// max size mapped by all the double indirect blocks
const size_t doubleIndirectBlkMaxSize = BLOCK_SIZE * blkAddrNos * blkAddrNos;

bmapResponse* bmap(inCoreiNode* inode, size_t offset) {
    // allocate size for bmap response
    bmapResponse* bmapResp = (bmapResponse*)malloc(sizeof(bmapResponse));
    bmapResp->byteOffsetInBlock = offset % BLOCK_SIZE;
    size_t numBytesToRead = BLOCK_SIZE - bmapResp->byteOffsetInBlock;
    bmapResp->ioBytesInBlock = numBytesToRead;
    //TODO: see if we need read ahead block number

    // the block number in which offset resides
    size_t blockNum;

    // indirection level
    int indirectionLevel = 0;
    // 1 if fetch block number from iNode
    int inodeBlkFlg = 1;

    // determing the level of indirection
    if (offset >= directBlkMaxSize && offset < singleIndirectBlkMaxSize)
        indirectionLevel = 1;
    else if (offset >= singleIndirectBlkMaxSize && offset < doubleIndirectBlkMaxSize)
        indirectionLevel = 2;
    else {
        // TODO:handle block not found error
        return NULL;
    }

    size_t newOffset = offset;
    int i = indirectionLevel;
    size_t blkSize = BLOCK_SIZE;

    while (i >= 0) {
        // calculating the index
        size_t index = newOffset / blkSize;

        // fetching block number from inode
        if (inodeBlkFlg) {
            if (indirectionLevel == 0)
                blockNum = inode->disk_iNode->direct_blocks[index];
            else if (indirectionLevel == 1)
                blockNum = inode->disk_iNode->single_indirect_blocks[0];
            else if (indirectionLevel == 2)
                blockNum = inode->disk_iNode->double_indirect_blocks[0];
            inodeBlkFlg = 0;
        } else if (indirectionLevel != 0) {
            // fetch the indirect block
            disk_block* metaBlock = (disk_block*)malloc(BLOCK_SIZE);
            metaBlock = getDiskBlock (blockNum, metaBlock);
            indirectBlock* diskBlock = (indirectBlock*)malloc(sizeof(indirectBlock));
            diskBlock = makeFreeDiskListBlock (metaBlock, diskBlock);

            // read the block number
            blockNum = diskBlock->blockNos[index];

            free(diskBlock);
            free(metaBlock);
        }

        // if reached final block number, return
        if (i == 0) {
            bmapResp->blockNumber = blockNum;
            return bmapResp;
        } else if (i == 1) {
            // resetting offset to start from beginning of single indirect block
            if (indirectionLevel == 1)
                newOffset = newOffset - directBlkMaxSize;
            else if (indirectionLevel == 2) {
                // offset - single indirect blocks covered till now
                newOffset = newOffset - singleIndirectBlkMaxSize * index;
                blkSize = BLOCK_SIZE;
            }
        } else if (i == 2) {
            // resetting offset to start from beginning of double indirect block
            newOffset = newOffset - singleIndirectBlkMaxSize;
            blkSize = BLOCK_SIZE * blkAddrNos;
        }

        i--;
    }
}
