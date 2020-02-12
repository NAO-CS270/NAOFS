#ifndef _PARAMS_H
#define _PARAMS_H

// TODO: Remove this variable later
#define DIRECT_BLOCKS 10ll

#define DISK_SIZE 1024ll

#define BLOCK_ADDRESS_SIZE 2ll  // affects definition of indirect block in bmap.h
#define INODE_ADDRESS_SIZE 1ll
#define FILENAME_SIZE 64ll

#define BLOCK_SIZE 1024ll
#define NUM_OF_BLOCKS (1ll<<(BLOCK_ADDRESS_SIZE*8ll))
#define INODE_SIZE (1ll<<7) // TODO: sizeof(iNode)?
#define NUM_OF_INODES (1ll<<(INODE_ADDRESS_SIZE*8ll))
#define DIRECT_BLOCK_LIMIT 9ll

#define SINGLE_INDIRECT_BLOCK 10ll
#define DOUBLE_INDIRECT_BLOCK 11ll
#define TRIPLE_INDIRECT_BLOCK 12ll

#define INODE_BUFFER_SIZE 32ll
#define INODE_HASH_SIZE 8ll

#define MAX_FD 40ll

#define BLOCK_ADDRESSES_PER_BLOCK (BLOCK_SIZE/BLOCK_ADDRESS_SIZE)
#define INODE_NOS_PER_BLOCK (BLOCK_SIZE/INODE_ADDRESS_SIZE)
#define INODES_PER_BLOCK (BLOCK_SIZE/INODE_SIZE)

#endif
