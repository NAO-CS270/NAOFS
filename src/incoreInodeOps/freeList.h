#ifndef NAOFS_LIST_H
#define NAOFS_LIST_H

#include "incoreInodeOps/node.h"
#include "inode/inCoreiNode.h"

// insert in front of the list
extern void freeList_insert(Node* head, Node* node);
// remove node from the freelist
extern void freeList_remove(Node* head, Node* node);

#endif //NAOFS_LIST_H
