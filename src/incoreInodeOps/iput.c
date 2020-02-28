#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/freeList.h"
#include "incoreInodeOps/node.h"
#include "incoreInodeOps/hashQ.h"
#include "inode/inCoreiNode.h"
#include "inode/iNode.h"
#include "mkfs/iNodeManager.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void writeINodeToDisk (inCoreiNode* inode) {
    iNode* diskINode = (iNode*)malloc(sizeof(iNode));
    getDiskINodeData(inode, diskINode);
    writeDiskInode(inode->inode_number, diskINode);
    free(diskINode);
}

int iput(inCoreiNode* inode) { //do we change the write time here?
	printf("iPut for %ld\n", inode->inode_number);
    printf("Sleeping on lock for iNode %ld\n", inode->inode_number);
    pthread_mutex_lock(&(inode->iNodeMutex));
    printf("Acquired lock for iNode %ld\n", inode->inode_number);
    if (inode->reference_count == 0) {
        pthread_mutex_unlock(&(inode->iNodeMutex));
        printf("Released lock for inode %ld\n", inode->inode_number);
        printf ("iPut - Reference count already zero\n");
        return -1;
    }
    inode->reference_count--;
	if(inode->reference_count == 0) {
        if (inode->inode_changed || inode->file_data_changed) {
            writeINodeToDisk(inode);
			inode->inode_changed = false;
			inode->file_data_changed = false;
        }

        Node* node = hashLookup(inode->inode_number, inode->device_number);
        if(node == NULL) {
            pthread_mutex_unlock(&(inode->iNodeMutex));
            printf("Released lock for inode %ld\n", inode->inode_number);
            printf ("iPut - iNode not present in hashQ\n");
            return -1;
        }

		printf("Inserting iNode %ld\n", inode->inode_number);
        freeListInsert(node);
    }
    pthread_mutex_unlock(&(inode->iNodeMutex));
	printf("Released lock for inode %ld\n", inode->inode_number);
    return 0;
}

