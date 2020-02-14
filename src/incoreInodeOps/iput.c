#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/freeList.h"

#include <stdio.h>

void iput(inCoreiNode* inode) {
    inode->reference_count--;
	printf("iPut for %ld\n", inode->inode_number);
	if(inode->reference_count == 0) {
		// Write inode to disk if needed.

        Node* node = hashLookup(inode->inode_number, inode->device_number);
        if(node == NULL) {
            // TODO: handle this exception
        }

		printf("Inserting iNode %ld\n", inode->inode_number);
        freeListInsert(node);
    }
    pthread_mutex_unlock(&(inode->iNodeMutex));
	printf("Released lock for inode %ld\n", inode->inode_number);
}

