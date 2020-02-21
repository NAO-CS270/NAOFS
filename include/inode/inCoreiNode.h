#ifndef INCOREINODE_H
#define INCOREINODE_H

#include "inode/iNode.h"

#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>

struct inCoreiNode {
    // mutex for this inode
    pthread_mutex_t iNodeMutex;

    // when this is 0, inode has to be put back into the inode free list.
    size_t reference_count;

    // device number
    size_t device_number;

    // inode number
    size_t inode_number; 

    // times accessed, modified and status changed
    time_t access;
    time_t modification;
    time_t status_change;

    // link count
    size_t linksCount;

    // File type and mode
	mode_t file_mode;

    // id of the owner
    size_t owner_uid;

    // if of the group
    size_t group_uid;

    // data blocks
    size_t dataBlockNums[BLOCKS_IN_INODE];

    // dirty bits for this inode
    bool inode_changed;
    bool file_data_changed;

    size_t size;
    // TODO: handle file mount point
    // TODO: A process is (or many processes are) waiting for it to be unlocked.
};

// Using this struct in the project. Keeping the old for for reference.
typedef struct inCoreiNode inCoreiNode;

// inserts disk inode data into the incore inode
extern void insertDiskInodeData(iNode* disk_inode, inCoreiNode* in_core);
extern void getDiskINodeData(inCoreiNode* in_core, iNode* disk_inode);

#endif //INCOREINODE_H

