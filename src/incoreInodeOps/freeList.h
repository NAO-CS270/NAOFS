#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "incoreInodeOps/node.h"
// #include "inode/inCoreiNode.h"

// initialize free in core inodes list
extern void initFreeInCoreINodeList();
// insert in front of the list
extern void freeListInsert(Node* node);
// remove node from the freelist
extern void freeListRemove(Node* node);
// get first incore inode from freelist
extern Node* getFreeINodeFromList();

#endif //FREE_LIST_H
