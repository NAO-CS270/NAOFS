#ifndef BMAP_H
#define BMAP_H

#include "inode/inCoreiNode.h"

#include <stddef.h>

/** blockNumber: the block number in which the offset resides
 *  byteOffsetInBlock: offset inside the block
 *	bytesLeftInBlock: bytes left to reach end of block if start from offset
 *  TODO: find what readAheadBlockNum it does
 */
struct bmapResponse {
    size_t blockNumber;
    size_t byteOffsetInBlock;
    size_t bytesLeftInBlock;
    size_t readAheadBlockNum;
};
typedef struct bmapResponse bmapResponse;

/**
 *	inode: In core copy of the inode
 *	offset: logical offset of byte wrt file
 *	returns bmapResponse: converts logical file offset to filesys offset
 */
extern void bmap(inCoreiNode* inode, size_t offset, bmapResponse *reponse);

#endif //BMAP_H

