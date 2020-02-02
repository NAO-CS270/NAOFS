#ifndef INCOREINODE_H
#define INCOREINODE_H

#include <stdbool.h>
#include "inode/iNode.h"

// Using this struct in the project. Keeping the old for for reference.
typedef struct inCoreiNode inCoreiNode;

struct inCoreiNode {
    bool lock;

    // when this is 0, inode has to be put back into the inode free list.
    size_t reference_count;

    // in case we have more than 1 file system
    size_t device_number;

    size_t inode_number;

    iNode* disk_iNode;
    // TODO: Add inode status?

};

extern void get_lock(inCoreiNode* iNode);
extern void release_lock(inCoreiNode* iNode);

extern void increment_reference_count(inCoreiNode* iNode);
extern void decrement_reference_count(inCoreiNode* iNode);

extern size_t get_reference_count(inCoreiNode* iNode);

#endif //INCOREINODE_H
