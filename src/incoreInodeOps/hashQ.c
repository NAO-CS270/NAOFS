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
size_t getHash(size_t deviceNumber, size_t inodeNumber) {
    return (deviceNumber + inodeNumber << 6 + inodeNumber << 16 - inodeNumber) % INODE_HASH_SIZE;
}

// insert into the hash table
void insertInHash(Node* node) {
    inCoreiNode* inode = node->inode;
    size_t index = getHash(inode->device_number, inode->inode_number);
    Node* head = hashQ[index];

    // change the hash pointers
    node->hash_next = head;
	if (head != NULL) {
		head->hash_prev = node;
	}
    node->hash_prev = NULL;
    hashQ[index] = node;
}

// returns the inode cached in the hash queue, NULL if not found
Node* hashLookup(size_t deviceNumber, size_t inodeNumber) {
    size_t hash_number = getHash(deviceNumber, inodeNumber);
    Node* head = hashQ[hash_number];
    while(head != NULL) {
		//printf("Hash lookup %ld\n", head->inode->inode_number);
        if(head->inode->inode_number == inodeNumber) {
            return head;
        }
        head = head->hash_next;
    }
    return NULL;
}

