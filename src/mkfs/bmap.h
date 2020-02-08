#ifndef BMAP_H
#define BMAP_H

#include <stdlib.h>
#include "inode/inCoreiNode.h"
#include "mkfs/metablocks.h"

struct bmapResponse {
    size_t blockNumber;
    size_t byteOffsetInBlock;
    size_t ioBytesInBlock;
    size_t readAheadBlockNum;
};

typedef struct bmapResponse bmapResponse;

// same struct for Free List Block and Indirect Block
typedef freeDiskListBlock indirectBlock;

extern bmapResponse* bmap(inCoreiNode* inode, size_t offset);
#endif //BMAP_H
