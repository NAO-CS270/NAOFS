#include "dsk/blkfetch.h"
#include "incoreInodeOps/iNodeManager.h"
#include "incoreInodeOps/bmap.h"
#include "mandsk/params.h"
#include "mkfs/metaBlocks.h"
#include "dsk/alloc.h"

#include <stdlib.h>

typedef freeDiskListBlock indirectBlock;

size_t processAndGetDiskBlock(blkTreeOffset *indirectionOffsets, size_t startBlockNum) {
	size_t indirectBlockIndex = startBlockNum;
	disk_block* dataBlock = (disk_block*)malloc(BLOCK_SIZE);
	indirectBlock* workingData = (indirectBlock*)malloc(sizeof(indirectBlock));

	size_t counter = 0;
	while(counter < indirectionOffsets->offsetIndirection) {
		getDiskBlock(indirectBlockIndex, dataBlock);
		makeFreeDiskListBlock(dataBlock, workingData);

		indirectBlockIndex = (workingData->blkNos)[indirectionOffsets->offsets[counter + 1]];
		counter++;
	}

	free(indirectionOffsets);
	free(dataBlock);
	free(workingData);

	return indirectBlockIndex;
}

void createBMapResponse(size_t blockNum, size_t offset, bmapResponse *response) {
	response->blockNumber = blockNum;
	response->byteOffsetInBlock = offset;
	response->bytesLeftInBlock = BLOCK_SIZE - offset;
}

void bmap(inCoreiNode* iNode, size_t offset, bmapResponse *response) {
	size_t fileSize = iNode->size;
	if (offset > fileSize) {
		free(response);
		response = NULL;
		return ;
	}
	if (offset == fileSize && (offset%BLOCK_SIZE) == 0) {
		size_t newBlock = blockAlloc();
		insertDataBlockInINode(iNode, newBlock);
		createBMapResponse(newBlock, 0, response);
		return ;
	}
	
	blkTreeOffset *indirectionOffsets = (blkTreeOffset *)malloc(sizeof(blkTreeOffset));
	calculateOffset(offset, indirectionOffsets);

	size_t dataBlockNum;
	if (indirectionOffsets->offsetIndirection == DIRECT) {
		dataBlockNum = indirectionOffsets->offsets[0];
	}
	else {
		size_t iNodeBlockIndex = DIRECT_BLOCK_LIMIT + indirectionOffsets->offsetIndirection;
		dataBlockNum = processAndGetDiskBlock(indirectionOffsets, iNode->dataBlockNums[iNodeBlockIndex]);
	}

	createBMapResponse(dataBlockNum, offset%BLOCK_SIZE, response);
}
