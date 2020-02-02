#ifndef HASHQ_H
#define HASHQ_H

#include <stdio.h>
#include "incoreInodeOps/node.h"
#include "mandsk/params.h"


// (device number + inode number) % INODE_HASH_SIZE
extern size_t getHash(size_t deviceNumber, size_t inodeNumber);

// insert into the hash table
extern void insertInHash(Node* node, Node** hashQ);
// search for the inode in the hashQ
extern Node* hashLookup(size_t deviceNumber, size_t inodeNumber, Node** hashQ);


#endif //HASHQ_H
