#ifndef _PARAMS_H
#define _PARAMS_H

#include <stddef.h>

#define BLOCK_ADDRESS_SIZE 32
#define INODE_ADDRESS_SIZE 32

#define BLOCK_SIZE 1024
#define NUM_OF_BLOCKS (1<<BLOCK_ADDRESS_SIZE)
#define INODE_SIZE (1<<4) // TODO: sizeof(iNode)?
#define NUM_OF_INODES (1<<INODE_ADDRESS_SIZE)

#define INODE_BUFFER_SIZE 32
#define INODE_HASH_SIZE 8

#define MAX_FD 40

#endif
