#include "dsk/node.h"
#include "dsk/hashQ.h"
#include "dsk/freeList.h"
#include "dsk/diskAccess.h"

#include <stddef.h>

cacheNode *getBlock(size_t blockNumber, size_t deviceNumber) {
	cacheNode *workingBuffer = lookupBufferCache(blockNumber, deviceNumber);

	if (workingBuffer != NULL) {
		pthread_mutex_lock(&(workingBuffer->header->bufferLock));
		cacheFreeListRemove(workingBuffer);
		return workingBuffer;
	}

	workingBuffer = getFreeBuffer();
	removeNodeFromCacheHashQ(workingBuffer);

	workingBuffer->header->blockNumber = blockNumber;
	workingBuffer->header->deviceNumber = deviceNumber;
	fetchDeviceDiskBlock(blockNumber, workingBuffer->dataBlock);

	insertNodeInCacheHashQ(workingBuffer);

	return workingBuffer;
}

void blockRelease(cacheNode *node) {
	writeDeviceDiskBlock(node->header->blockNumber, node->dataBlock);
	node->header->delayedWrite = false;
	cacheFreeListInsert(node);
	pthread_mutex_unlock(&(node->header->bufferLock));
}

