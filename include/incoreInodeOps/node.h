#ifndef NODE_H
#define NODE_H
#include "inode/inCoreiNode.h"

struct Node {
    inCoreiNode* inode;
    struct Node* prev;
    struct Node* next;

    // pointers to the hash queue containing these inodes
    struct Node* hash_prev;
    struct Node* hash_next;
};

typedef struct Node Node;

#endif //NODE_H
