#include "dsk/node.h"
#include "dsk/cacheParams.h"
#include "dsk/diskAccess.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

/* This is a doubly-linked circular linked list. */
static cacheNode* freeList;

static pthread_mutex_t emptyListLock = PTHREAD_MUTEX_INITIALIZER;

void cacheFreeListInsert(cacheNode* node) {
	pthread_mutex_unlock(&(node->header->bufferLock));
	
	if (freeList == NULL) {
		freeList = node;
		node->next = freeList;
		node->prev = freeList;
		pthread_mutex_unlock(&emptyListLock);
		return ;
	}
	node->next = freeList;
	node->prev = freeList->prev;
	freeList->prev->next = node;
	freeList->prev = node;
}

void initBufferCacheFreeList() {
    freeList = NULL;
    int counter;
    for(counter = 0; counter < FREE_LIST_SIZE ; counter++) {
        cacheNode* node = (cacheNode *)malloc(sizeof(cacheNode));
		memset(node, 0, sizeof(cacheNode));

		node->header = (blockHeader *)malloc(sizeof(blockHeader));
		memset(node->header, 0, sizeof(blockHeader));
		pthread_mutex_init(&(node->header->bufferLock), NULL);

        node->dataBlock = (disk_block *)malloc(sizeof(disk_block));
		memset(node->dataBlock, 0, sizeof(disk_block));

		node->hash_next = NULL;
		node->hash_prev = NULL;

		cacheFreeListInsert(node);
    }
}

void cacheFreeListRemove(cacheNode* node) {
    cacheNode* workingNode = freeList;
	if (freeList != NULL && node == freeList && freeList->next == freeList) {
		freeList = NULL;
		node->next = NULL;
		node->prev = NULL;
		return ;
	}
    while(workingNode != NULL) {
		if (node == workingNode) {
			if (freeList == node) {
				freeList = node->next;
			}
			node->next->prev = node->prev;
			node->prev->next = node->next;
			node->next = NULL;
			node->prev = NULL;
			break;
		}
		workingNode = workingNode->next;
		if (workingNode == freeList) {
			break;
		}
	}
}

cacheNode *popCacheFreeList() {
	if (freeList == NULL) {
		return NULL;
	}

	cacheNode *toReturn = freeList;
	if (freeList->next == freeList) {
		freeList = NULL;
		return toReturn;
	}
	else {
		freeList->prev->next = freeList->next;
		freeList->next->prev = freeList->prev;
		freeList = freeList->next;
	}
	toReturn->next = NULL;
	toReturn->prev = NULL;
	return toReturn;
}

cacheNode *getFreeBuffer() {
	cacheNode *freeBuffer = NULL;

	while (freeBuffer == NULL) {
		freeBuffer = popCacheFreeList();
		if (freeBuffer == NULL) {
			pthread_mutex_lock(&emptyListLock);
		}
	}

	pthread_mutex_lock(&(freeBuffer->header->bufferLock));
	cacheFreeListRemove(freeBuffer);
	if (freeBuffer->header->delayedWrite) {
		// TODO - Make this synchronous and invoke in loop
		writeDeviceDiskBlock(freeBuffer->header->blockNumber, freeBuffer->dataBlock);
	}

	return freeBuffer;
}

