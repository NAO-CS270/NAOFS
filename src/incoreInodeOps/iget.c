#include "incoreInodeOps/iget.h"
#include "time.h"
#include "utils/utils.h"

#include <stdio.h>

void fetchInodeFromDisk(size_t iNodeNumber, inCoreiNode* inode) {
    iNode* disk_inode = (iNode*)malloc(sizeof(iNode));
    getDiskInode(iNodeNumber, disk_inode);
    insertDiskInodeData(disk_inode, inode);
    free(disk_inode);
}

inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber) {
    // looking for the inode in the hash Q
    Node* node = hashLookup(deviceNumber, iNodeNumber);
    inCoreiNode* fetchediNode;
    if(NULL != node) {
        fetchediNode = node->inode;
        // get a lock on the inode, wait until another process releases it
        pthread_mutex_lock(&(fetchediNode->iNodeMutex));
        // TODO: special processing for mount points.

        // if also in free in list, remove it
        if(fetchediNode->reference_count == 0) {
            freeListRemove(node);
        }
        fetchediNode->reference_count++;
        // returning locked inode
    }
    else {
        if (checkFreeListEmpty()) {
			printf("iGet returning NULL as Free List is empty\n");
            // TODO: Error handling
            return NULL;
        }
        // getting free inode from free list and removing it
        node = getFreeINodeFromList();
        node->inode->inode_number = iNodeNumber;
        node->inode->device_number = deviceNumber;

        freeListRemove(node);
        // updating the hash Q entry with new inode
        insertInHash(node);
        fetchInodeFromDisk(iNodeNumber, node->inode);
        // this lock has to be released by the process using iget
        pthread_mutex_lock(&(node->inode->iNodeMutex));
        node->inode->reference_count++;
        fetchediNode = node -> inode;
    }
    fetchediNode -> inode_changed = true;
	printf("Returning iNode number %d\n", fetchediNode->inode_number);

    return fetchediNode;
}
