#ifndef HASHQ_H
#define HASHQ_H

#include "incoreInodeOps/node.h"
#include "mandsk/params.h"

#include <stddef.h>

extern void initHashQueues();

// (device number + inode number) % INODE_HASH_SIZE
extern size_t getHash(size_t deviceNumber, size_t inodeNumber);

extern void updateInHashQ(Node *node, size_t iNodeNumber, size_t deviceNumber);

// search for the inode in the hashQ
extern Node* hashLookup(size_t deviceNumber, size_t inodeNumber);

#endif

