#include "incoreInodeOps/iget.h"
#include "inode/iNode.h"

#include <stdio.h>
#include <pthread.h>

void fetchInodeFromDisk(size_t iNodeNumber, inCoreiNode* inode) {
    iNode* disk_inode = (iNode*)malloc(sizeof(iNode));
    getDiskInode(iNodeNumber, disk_inode);

    insertDiskInodeData(disk_inode, inode);
    free(disk_inode);
}

inCoreiNode *updateAndGetINode(Node *coreNode) {
	inCoreiNode *iNodeToReturn = coreNode->inode;

	printf("Sleeping on lock for iNode %ld\n", iNodeToReturn->inode_number);
	pthread_mutex_lock(&(iNodeToReturn->iNodeMutex));
	printf("Acquired lock for iNode %ld\n", iNodeToReturn->inode_number);

	(iNodeToReturn->reference_count)++;
	return iNodeToReturn;
}

inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber) {
	printf("iGet iNode %ld\n", iNodeNumber);
    Node* node = hashLookup(iNodeNumber, deviceNumber);

    if(node == NULL) {
		printf("iNode not in HashQ\n");
		
		node = popFreeList();
		if (node == NULL) {
			return NULL;
		}
		updateInHashQ(node, iNodeNumber, deviceNumber);
		fetchInodeFromDisk(iNodeNumber, node->inode);
    }
	else {
		if (node->inode->reference_count == 0) {
			freeListRemove(node);
		}
	}

	return updateAndGetINode(node);
}

