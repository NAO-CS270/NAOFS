#include "dsk/node.h"
#include "dsk/blkfetch.h"
#include "incoreInodeOps/iNodeManager.h"
#include "incoreInodeOps/bmap.h"
#include "mandsk/params.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/alloc.h"

#include <stdio.h>
#include <stdlib.h>

typedef freeDiskListBlock indirectBlock;

size_t processAndGetDiskBlock(blkTreeOffset *indirectionOffsets, size_t startBlockNum) {
	size_t indirectBlockIndex = startBlockNum;
	cacheNode *dataBlockNode;
	indirectBlock* workingData = (indirectBlock*)malloc(sizeof(indirectBlock));

	size_t counter = 0;
	while(counter < indirectionOffsets->offsetIndirection) {
		dataBlockNode = getDiskBlockNode(indirectBlockIndex, 0);
		makeFreeDiskListBlock(dataBlockNode->dataBlock, workingData);

		indirectBlockIndex = (workingData->blkNos)[indirectionOffsets->offsets[counter + 1]];
		writeDiskBlockNode(dataBlockNode);
		counter++;
	}

	free(workingData);

	return indirectBlockIndex;
}

void createBMapResponse(size_t blockNum, size_t offset, bmapResponse *response) {
	response->blockNumber = blockNum;
	response->byteOffsetInBlock = offset;
	response->bytesLeftInBlock = BLOCK_SIZE - offset;
}

int bmap(inCoreiNode* iNode, size_t offset, bmapResponse *response, bmapMode mode) {
	size_t fileSize = iNode->size;
	printf("Inside bmap, filesize - %ld, offset - %ld\n", fileSize, offset);
	if (offset >= fileSize) {
	    if (mode == READ_MODE || offset > fileSize) {
	        printf("bmap: illegal Offset access\n");
            return -1;
        }
	}
	printf("pass first error check");
	if (offset == fileSize && (offset%BLOCK_SIZE) == 0 && mode == APPEND_MODE) {
		size_t newBlock = blockAlloc();
		printf("Allocated new block in bmap - %ld\n", newBlock);
		insertDataBlockInINode(iNode, newBlock);

		int counter = 0;
		printf("\n");
		while (counter < BLOCKS_IN_INODE) {
			printf("%ld ", iNode -> dataBlockNums[counter++]);
		}
		printf("\n");

		createBMapResponse(newBlock, 0, response);
		return 0;
	}
	
	blkTreeOffset *indirectionOffsets = (blkTreeOffset *)malloc(sizeof(blkTreeOffset));
	calculateOffset(offset, indirectionOffsets);

	size_t dataBlockNum;
	if (indirectionOffsets->offsetIndirection == DIRECT) {
		dataBlockNum = iNode->dataBlockNums[indirectionOffsets->offsets[0]];
	}
	else {
		size_t iNodeBlockIndex = DIRECT_BLOCK_LIMIT + indirectionOffsets->offsetIndirection;
		dataBlockNum = processAndGetDiskBlock(indirectionOffsets, iNode->dataBlockNums[iNodeBlockIndex]);
	}

	createBMapResponse(dataBlockNum, offset%BLOCK_SIZE, response);

	free(indirectionOffsets);
	return 0;
}
