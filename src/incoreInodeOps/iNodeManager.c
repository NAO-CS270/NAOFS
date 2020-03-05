#include "incoreInodeOps/iNodeManager.h"

#include "dsk/blkfetch.h"
#include "dsk/alloc.h"
#include "mandsk/params.h"
#include "mkfs/metaBlocks.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR_BLOCK 0

// use same struct for free disk blk and indirect blk
typedef freeDiskListBlock indirectBlock;

static const size_t blkAddrNos = BLOCK_ADDRESSES_PER_BLOCK;

#define SINGLE_INDIRECT_LIMIT (DIRECT_BLOCK_LIMIT + BLOCK_ADDRESSES_PER_BLOCK)
#define DOUBLE_INDIRECT_LIMIT (SINGLE_INDIRECT_LIMIT + (BLOCK_ADDRESSES_PER_BLOCK * BLOCK_ADDRESSES_PER_BLOCK))
#define TRIPLE_INDIRECT_LIMIT (DOUBLE_INDIRECT_LIMIT + (BLOCK_ADDRESSES_PER_BLOCK * BLOCK_ADDRESSES_PER_BLOCK * BLOCK_ADDRESSES_PER_BLOCK))

void blkTreeOffsetConstructor(int directOffset, int firstOffset, int secondOffset, int thirdOffset, indirectionLevel indirection, blkTreeOffset *blkOffset) {
	blkOffset->offsets[DIRECT] = directOffset;
	blkOffset->offsets[SINGLE_INDIRECT] = firstOffset;
	blkOffset->offsets[DOUBLE_INDIRECT] = secondOffset;
	blkOffset->offsets[TRIPLE_INDIRECT] = thirdOffset;
	blkOffset->offsetIndirection = indirection;
}

/**
 *	Calculates different offsets based on the given file offset
 *	blkOffset: the block tree offset struct
 *	offset: offset of the file
 */
void calculateOffset(size_t offset, blkTreeOffset* blkOffset) {
	size_t blkIndex = offset / BLOCK_SIZE;
	if (blkIndex <= DIRECT_BLOCK_LIMIT) {
		blkTreeOffsetConstructor(blkIndex, -1, -1, -1, DIRECT, blkOffset);
	}
	else if (blkIndex <= SINGLE_INDIRECT_LIMIT) {
		size_t firstOffset = blkIndex - DIRECT_BLOCK_LIMIT - 1;
		blkTreeOffsetConstructor(-1, firstOffset, -1, -1, SINGLE_INDIRECT, blkOffset);
	}
	else if (blkIndex <= DOUBLE_INDIRECT_LIMIT) {
		size_t firstOffset = (blkIndex - SINGLE_INDIRECT_LIMIT - 1) / blkAddrNos;
		size_t secondOffset = (blkIndex - (firstOffset * blkAddrNos)) - SINGLE_INDIRECT_LIMIT - 1;
		blkTreeOffsetConstructor(-1, firstOffset, secondOffset, -1, DOUBLE_INDIRECT, blkOffset);
	}
	else if (blkIndex <= TRIPLE_INDIRECT_LIMIT) {
		size_t firstOffset = (blkIndex - DOUBLE_INDIRECT_LIMIT - 1) / (blkAddrNos * blkAddrNos);
		size_t secondOffset = (blkIndex - (firstOffset * (blkAddrNos * blkAddrNos)) - DOUBLE_INDIRECT_LIMIT - 1) / blkAddrNos;
		size_t thirdOffset = blkIndex - secondOffset * blkAddrNos - firstOffset * (blkAddrNos * blkAddrNos) - DOUBLE_INDIRECT_LIMIT - 1;
		blkTreeOffsetConstructor(-1, firstOffset, secondOffset, thirdOffset, TRIPLE_INDIRECT, blkOffset);
	}
}

size_t allocateIfNeeded(int *indirOffsets, size_t offsetsSize) {
	size_t counter = 0;
	bool shouldAdd = true;
	while (counter < offsetsSize) {
		if (indirOffsets[counter] != 0) {
			shouldAdd = false;
			break;
		}
		counter++;
	}

	if (shouldAdd) {
		printf("block alloc needed\n");
		return blockAlloc();
	}
	return ERROR_BLOCK;
}

void allocateAllNeededBlocks(size_t *curBlock, size_t blockNumToAdd, int *indirOffsets, size_t offsetsSize) {
	size_t counter = 0;
	size_t parentBlock;
	size_t newAllocBlock;
	
	disk_block* dataBlock = (disk_block*)malloc(BLOCK_SIZE);
	indirectBlock* workingData = (indirectBlock*)malloc(sizeof(indirectBlock));
	
	printf("allocating needed blocks with buffer: %d %d %d and buffer size %d\n", indirOffsets[0], indirOffsets[1], indirOffsets[2], offsetsSize);
	printf("initial blk %d toBeAdded block %d\n", *curBlock, blockNumToAdd);
	while (counter < offsetsSize) {
		newAllocBlock = allocateIfNeeded(indirOffsets + counter, offsetsSize - counter);
		
		if (ERROR_BLOCK != newAllocBlock) {
			*curBlock = newAllocBlock;

			if (counter != 0) {
				writeFreeDiskListBlock(workingData, dataBlock);
				writeDiskBlock(parentBlock, dataBlock);
				printf("inside nested if with blkNum %d & curBlock %d\n", parentBlock, *curBlock);
			}
		}
		size_t tmp = *curBlock;
		getDiskBlock(*curBlock, dataBlock);
		makeFreeDiskListBlock(dataBlock, workingData);

		parentBlock = tmp;
		printf("parentBlock set to %d\n", parentBlock);
		curBlock = (workingData->blkNos) + indirOffsets[counter];
		
		counter ++;
	}
	*curBlock = blockNumToAdd;
	writeFreeDiskListBlock(workingData, dataBlock);
	writeDiskBlock(parentBlock, dataBlock);
	printf("at the end blkNum %d\n", parentBlock);

	free(dataBlock);
	free(workingData);
}

/**
 *	Helper function for insertDataBlockINode
 *	inode: in-core copy of the inode
 *	blockNumToAdd: the address of the newly allocated block
 *	blkOffset: Holds the offsets for each indirect block
 */
void updateIndex(inCoreiNode* iNode, size_t blockNumToAdd, blkTreeOffset *blkOffset) {
	if (blkOffset->offsetIndirection == DIRECT) {
		iNode->dataBlockNums[blkOffset->offsets[DIRECT]] = blockNumToAdd;
		return ;
	}

	int *offsets = blkOffset->offsets;
	size_t indirection = blkOffset->offsetIndirection;
	size_t *dataBlockIndex = iNode->dataBlockNums + DIRECT_BLOCK_LIMIT + indirection;
	
	allocateAllNeededBlocks(dataBlockIndex, blockNumToAdd, offsets + 1, indirection);

	updateINodeMetadata(iNode, 0, iNode->linksCount);
}

/**
 *	Updates in-core inode data blocks list with the newly allocated block
 *	When this function is called the size should point at the head of new block
 * 	=> (size % BLOCK_SIZE) == 0
 * 	inode: in-core copy of inode
 *	blockNumToAdd: block to address to add
 */
void insertDataBlockInINode(inCoreiNode* iNode, size_t blockNumToAdd) {
	printf("Adding block %ld in iNode %ld", blockNumToAdd, iNode->inode_number);
	size_t size = iNode->size;
	if (size % BLOCK_SIZE != 0) {
		// TODO: handle error
		return ;
	}

	blkTreeOffset * blkOffset = (blkTreeOffset *)malloc(sizeof(blkTreeOffset));
	calculateOffset(size, blkOffset);

	updateIndex(iNode, blockNumToAdd, blkOffset);
	free(blkOffset);
}

void updateINodeMetadata(inCoreiNode *iNode, int sizeDifference, size_t linkCount) {
	iNode->size += sizeDifference;
	iNode->linksCount = linkCount;

	iNode->modification = time(NULL);

	iNode->file_data_changed = true;
	iNode->inode_changed = true;
}

