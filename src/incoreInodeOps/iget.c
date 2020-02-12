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

	if (iNodeToReturn->reference_count == 0) {
		freeListRemove(coreNode);
	}
	(iNodeToReturn->reference_count)++;
	iNodeToReturn->inode_changed = true;
	return iNodeToReturn;
}

inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber) {
    Node* node = hashLookup(deviceNumber, iNodeNumber);

    if(NULL == node) {
		node = getFreeINodeFromList();
		fetchInodeFromDisk(iNodeNumber, node->inode);
		insertInHash(node);
    }

	return updateAndGetINode(node);
}

