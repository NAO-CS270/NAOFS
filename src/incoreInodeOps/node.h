#ifndef PROJECT1_NODE_H
#define PROJECT1_NODE_H
#include "incoreInodeOps/node.h"

struct Node {
    inCoreiNode* inode;
    struct Node* prev;
    struct Node* next;

    // pointers to the hash queue containing these inodes
    struct Node* hash_prev;
    struct Node* hash_next;
};

typedef struct Node Node;

#endif //PROJECT1_NODE_H
