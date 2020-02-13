#include "incoreInodeOps/hashQ.h"

#include <stdio.h>

static Node* hashQ[INODE_HASH_SIZE];

void initHashQueues() {
	size_t counter;

	while (counter < INODE_HASH_SIZE) {
		hashQ[counter] = NULL;
		counter ++;
	}
}

// returns the index of the inode in hashQ
size_t getHash(size_t iNodeNumber, size_t deviceNumber) {
    return (deviceNumber + iNodeNumber << 6 + iNodeNumber << 16 - iNodeNumber) % INODE_HASH_SIZE;
}

void removeFromHashQ(Node *node) {
	size_t hashIndex = getHash(node->inode->inode_number, node->inode->device_number);
	
	if (hashQ[hashIndex] == node) {
		hashQ[hashIndex] = node->hash_next;
	}
	if (node->hash_next != NULL) {
		node->hash_next->hash_prev = node->hash_prev;
	}
	if (node->hash_prev != NULL) {
		node->hash_prev->hash_next = node->hash_next;
	}

	node->hash_next = NULL;
	node->hash_prev = NULL;
}

void insertInHash(Node *node, size_t iNodeNumber, size_t deviceNumber) {
    size_t hashIndex = getHash(iNodeNumber, deviceNumber);
    Node* head = hashQ[hashIndex];

    node->hash_next = head;
	if (head != NULL) {
		head->hash_prev = node;
	}
    node->hash_prev = NULL;
    hashQ[hashIndex] = node;
}

void updateInHashQ(Node *node, size_t iNodeNumber, size_t deviceNumber) {
	removeFromHashQ(node);

	insertInHash(node, iNodeNumber, deviceNumber);
}

// returns the inode cached in the hash queue, NULL if not found
Node* hashLookup(size_t iNodeNumber, size_t deviceNumber) {
    size_t hash_number = getHash(iNodeNumber, deviceNumber);
	printf("iNode number %ld is mapped to queueu number %ld\n", iNodeNumber, hash_number);
    Node* head = hashQ[hash_number];
    while(head != NULL) {
        if(head->inode->inode_number == iNodeNumber) {
            return head;
        }
        head = head->hash_next;
    }
    return NULL;
}

