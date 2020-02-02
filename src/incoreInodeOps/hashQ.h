#ifndef NAOFS_HASHQ_H
#define NAOFS_HASHQ_H

#include <stdio.h>
#include "incoreInodeOps/list.h"
#include "incoreInodeOps/node.h"
#include "mandsk/params.h"

//
//struct hashQ {
//    Node* hash_array[INODE_HASH_SIZE];
//};
//
//typedef struct hashQ hashQ;

// (device number + inode number) % INODE_HASH_SIZE
extern size_t get_hash(size_t device_number, size_t inode_number);

// insert into the hash table
extern void insert(Node* node, Node** hashQ);
// search for the inode in the hashQ
extern Node* hash_lookup(size_t deviceNumber, size_t inodeNumber, Node** hashQ);


#endif //NAOFS_HASHQ_H
