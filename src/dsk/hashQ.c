#include "dsk/node.h"
#include "dsk/cacheParams.h"

#include <stddef.h>

static cacheNode* hashQ[HASH_Q_SIZE];

void initCacheHashQueues() {
	size_t counter;

	while (counter < HASH_Q_SIZE) {
		hashQ[counter] = NULL;
		counter ++;
	}
}

size_t getCacheHash(size_t blockNumber, size_t deviceNumber) {
    return (deviceNumber + blockNumber << 6 + blockNumber << 16 - blockNumber) % HASH_Q_SIZE;
}

void removeNodeFromCacheHashQ(cacheNode *node) {
	size_t hashIndex = getCacheHash(node->header->blockNumber, node->header->deviceNumber);
	
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

void insertNodeInCacheHashQ(cacheNode *node) {
	size_t hashIndex = getCacheHash(node->header->blockNumber, node->header->deviceNumber);
    cacheNode* head = hashQ[hashIndex];

    node->hash_next = head;
	if (head != NULL) {
		head->hash_prev = node;
	}
    node->hash_prev = NULL;
    hashQ[hashIndex] = node;
}

// returns the inode cached in the hash queue, NULL if not found
cacheNode* lookupBufferCache(size_t blockNumber, size_t deviceNumber) {
    size_t hash_number = getCacheHash(blockNumber, deviceNumber);
    cacheNode* nodeIterator = hashQ[hash_number];

    while(nodeIterator != NULL) {
        if((nodeIterator->header)->blockNumber == blockNumber) {
            return nodeIterator;
        }
        nodeIterator = nodeIterator->hash_next;
    }
    return NULL;
}

