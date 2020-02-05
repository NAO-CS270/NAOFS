#ifndef _SPEC_BLOCKS_H
#define _SPEC_BLOCKS_H

#include "dsk/mdisk.h"
#include "mandsk/params.h"

#define INODE_NOS_PER_BLOCK (BLOCK_SIZE/INODE_ADDRESS_SIZE)

struct superBlock {
	size_t rememberedINodeNum;
};
typedef struct superBlock superBlock;

struct freeDiskListBlock {
};
typedef struct freeDiskListBlock freeDiskListBlock;

struct iNodeListBlock {
	size_t iNodeNos[INODE_NOS_PER_BLOCK];
};
typedef struct iNodeListBlock iNodeListBlock;

extern iNodeListBlock *makeINodeListBlock(disk_block *blockPtr);

#undef INODE_NOS_PER_BLOCK

#endif
