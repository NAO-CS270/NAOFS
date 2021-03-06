#ifndef _PARAMS_H
#define _PARAMS_H

// TODO: Remove this variable later
#define DIRECT_BLOCKS 10ll

#include <stddef.h>

#define BLOCK_ADDRESS_SIZE 8ll
#define DISK_SIZE_IN_GB 30ll

#define BLOCK_SIZE 1024ll

#define INODE_ADDRESS_SIZE 2ll
#define NUM_OF_INODES (1ll<<(INODE_ADDRESS_SIZE*8ll))
#define FILENAME_SIZE 62ll
#define DIRECTORY_ENTRY_SIZE (FILENAME_SIZE + INODE_ADDRESS_SIZE)
#define DIRECTORY_ENTRIES_IN_BLOCK (BLOCK_SIZE / DIRECTORY_ENTRY_SIZE)

// TODO: Remove this variable later
#define DIRECT_BLOCKS 10ll
#define SINGLE_INDIRECT_BLOCK 10ll
#define DOUBLE_INDIRECT_BLOCK 11ll
#define TRIPLE_INDIRECT_BLOCK 12ll

#define DIRECT_BLOCK_LIMIT 9ll

#define INODE_BUFFER_SIZE 32ll
#define INODE_HASH_SIZE 8ll

// #define MAX_FD 40ll

#define MAX_FILE_DESCRIPTORS 10000ll

#define BLOCK_ADDRESSES_PER_BLOCK (BLOCK_SIZE/BLOCK_ADDRESS_SIZE)
#define INODE_NOS_PER_BLOCK (BLOCK_SIZE/INODE_ADDRESS_SIZE)

extern size_t INODE_SIZE;
extern size_t INODES_PER_BLOCK;
extern size_t NUM_OF_INODE_BLOCKS;

extern size_t NUM_OF_BLOCKS;

extern void initializeINodeParams();

#endif

