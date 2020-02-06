#ifndef _SPEC_BLOCKS_H
#define _SPEC_BLOCKS_H

#include "dsk/mdisk.h"
#include "mandsk/params.h"
#include "inode/iNode.h"

#define INODE_NOS_PER_BLOCK (BLOCK_SIZE/INODE_ADDRESS_SIZE)
#define INODES_PER_BLOCK (BLOCK_SIZE/INODE_SIZE)

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

struct iNodesBlock {
	iNode iNodesList[INODES_PER_BLOCK];
};
typedef struct iNodesBlock iNodesBlock;

extern superBlock *makeSuperBlock(disk_block *blockPtr, superBlock *theBlock);

extern iNodeListBlock *makeINodeListBlock(disk_block *blockPtr, iNodeListBlock *theBlock);

extern iNodesBlock *makeINodesBlock(disk_block *blockPtr, iNodesBlock *theBlock);

#undef INODE_NOS_PER_BLOCK
#undef INODES_PER_BLOCK

#endif
