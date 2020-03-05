#include "incoreInodeOps/iget.h"
#include "inode/iNode.h"

#include <time.h>
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

	printf("Ref count before increment %ld, iNodeNum: %ld\n", iNodeToReturn->reference_count, iNodeToReturn->inode_number);
	(iNodeToReturn->reference_count) += 1;
	printf("Ref count before increment %ld, iNodeNum: %ld\n", iNodeToReturn->reference_count, iNodeToReturn->inode_number);
	pthread_mutex_unlock(&(iNodeToReturn->iNodeMutex));
	printf("Released lock for inode %ld\n", iNodeToReturn->inode_number);
	return iNodeToReturn;
}

inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber) {
	printf("\n------------------------\niGet iNode %ld\n\n", iNodeNumber);
    Node* node = hashLookup(iNodeNumber, deviceNumber);

    if(node == NULL) {
		printf("iNode not in HashQ\n");
		
		node = popFreeList();
		if (node == NULL) {
			return NULL;
		}
		printf("Sleeping on lock for iNode %ld\n", iNodeNumber);
		pthread_mutex_lock(&(node->inode->iNodeMutex));
		printf("Acquired lock for iNode %ld\n", iNodeNumber);
		updateInHashQ(node, iNodeNumber, deviceNumber);
		fetchInodeFromDisk(iNodeNumber, node->inode);
    }
	else {
		printf("Sleeping on lock for iNode %ld\n", node->inode->inode_number);
		pthread_mutex_lock(&(node->inode->iNodeMutex));
		printf("Acquired lock for iNode %ld\n", node->inode->inode_number);
		if (node->inode->reference_count == 0) {
			printf("Removing iNode %ld from free list\n", node->inode->inode_number);
			freeListRemove(node);
		}
		if (node->inode->linksCount == 0) {
			fetchInodeFromDisk(iNodeNumber, node->inode);
		}
	}

	return updateAndGetINode(node);
}

