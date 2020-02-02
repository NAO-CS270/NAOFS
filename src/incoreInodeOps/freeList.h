#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "incoreInodeOps/node.h"
#include "inode/inCoreiNode.h"

// insert in front of the list
extern void freeListInsert(Node* head, Node* node);
// remove node from the freelist
extern void freeListRemove(Node* head, Node* node);

#endif //FREE_LIST_H
