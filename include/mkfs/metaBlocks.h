#ifndef _SPEC_BLOCKS_H
#define _SPEC_BLOCKS_H

#include "dsk/mdisk.h"
#include "mandsk/params.h"
#include "inode/iNode.h"

#include "dsk/mdisk.h"
#include "inode/inCoreiNode.h"

// TODO: initialize the superblock as part of mkfs.
struct superBlock {
    size_t file_system_size;

    size_t num_free_inodes;
    size_t* free_inode_list;
    size_t next_free_inode;
    size_t remembered_inode;

    bool dirty_bit;
};
typedef struct superBlock superBlock;

// use same struct for indirect block
struct freeDiskListBlock {
	size_t blkNos[BLOCK_ADDRESSES_PER_BLOCK];
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
extern disk_block *writeSuperBlock(superBlock *theBlock, disk_block *blockPtr);

extern freeDiskListBlock* makeFreeDiskListBlock(disk_block* blockPtr, freeDiskListBlock* theBlock);
extern disk_block* writeFreeDiskListBlock(freeDiskListBlock* theBlock, disk_block* blockPtr);

extern iNodeListBlock *makeINodeListBlock(disk_block *blockPtr, iNodeListBlock *theBlock);
extern disk_block *writeINodeListBlock(iNodeListBlock *theBlock, disk_block *blockPtr);

extern iNodesBlock *makeINodesBlock(disk_block *blockPtr, iNodesBlock *theBlock);
extern disk_block *writeINodesBlock(iNodesBlock *theBlock, disk_block *blockPtr);

#endif
