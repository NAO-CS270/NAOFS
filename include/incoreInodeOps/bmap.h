#ifndef BMAP_H
#define BMAP_H

#include <stdlib.h>
#include "inode/inCoreiNode.h"
#include "mkfs/metaBlocks.h"

/** blockNumber: the block number in which the offset resides
 *  byteOffsetInBlock: offset inside the block
 *	ioBytesInBlock: bytes left to reach end of block if start from offset
 *  TODO: find what readAheadBlockNum it does
 */
struct bmapResponse {
    size_t blockNumber;
    size_t byteOffsetInBlock;
    size_t ioBytesInBlock;
    size_t readAheadBlockNum;
};

typedef struct bmapResponse bmapResponse;

// same struct for Free List Block and Indirect Block
typedef freeDiskListBlock indirectBlock;

/**
 *	inode: In core copy of the inode
 *	offset: logical offset of byte wrt file
 *	returns bmapResponse: converts logical file offset to filesys offset
 */
extern bmapResponse* bmap(inCoreiNode* inode, size_t offset);
#endif //BMAP_H
