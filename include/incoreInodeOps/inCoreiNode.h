#ifndef INCOREINODE_H
#define INCOREINODE_H

#include <pthread.h>
#include <stdbool.h>
#include "inode/iNode.h"

// Using this struct in the project. Keeping the old for for reference.
typedef struct inCoreiNode inCoreiNode;

struct inCoreiNode {
    // mutex for this inode
    pthread_mutex_t iNodeMutex;

    // when this is 0, inode has to be put back into the inode free list.
    size_t reference_count;

    // in case we have more than 1 file system
    size_t device_number;

    size_t inode_number;

    iNode* disk_iNode;

    // dirty bits for this inode
    bool inode_changed;
    bool file_data_changed;
    bool lock; // TODO: IS IT CORRECT?
    // TODO: handle file mount point
    // TODO: A process is (or many processes are) waiting for it to be unlocked.

};

// Using this struct in the project. Keeping the old for for reference.
typedef struct inCoreiNode inCoreiNode;

#endif //INCOREINODE_H
