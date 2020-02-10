#include "incoreInodeOps/iget.h"

void fetchInodeFromDisk(size_t iNodeNumber, inCoreiNode* inode) {
    iNode* disk_inode = (iNode*)malloc(sizeof(iNode));
    getDiskInode(iNodeNumber, disk_inode);
    insertDiskInodeData(disk_inode, inode);
    free(disk_inode);
}

inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber) {
    // looking for the inode in the hash Q
    Node* node = hashLookup(deviceNumber, iNodeNumber);
    if(NULL != node) {
        inCoreiNode* inode = node->inode;
        // get a lock on the inode, wait until another process releases it
        pthread_mutex_lock(&(inode->iNodeMutex));
        // TODO: special processing for mount points.

        // if also in free in list, remove it
        if(inode->reference_count == 0) {
            freeListRemove(node);
        }
        inode->reference_count++;
        // returning locked inode
        return inode;
    }
    if(checkFreeListEmpty()) {
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

    // TODO: Update inode status?
    return node->inode;
}
