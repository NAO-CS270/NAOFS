#include <stddef.h>
#include <stdlib.h>

#include "dsk/blkfetch.h"
#include "mandsk/params.h"
#include "freeBlockList.h"

static size_t blockAddressSize = BLOCK_ADDRESS_SIZE;
static size_t inodeAddressSize = INODE_ADDRESS_SIZE;

static size_t blockSize = BLOCK_SIZE;
static size_t numOfBlocks = NUM_OF_BLOCKS;
static size_t inodeSize = INODE_SIZE;
static size_t numOfInodes = NUM_OF_INODES;

void makeFileSystem() {
	size_t permBlocks = 4;

	size_t numOfInodeBlocks = (numOfInodes * inodeSize) / blockSize;

	initializeDiskBlocks(3, 4 + numOfInodeBlocks);
}

/* Sets up the free block list structure, assuming that the free data blocks start
 * at `startPos`, and the super block free list block is at `freeBlockNum`. A block
 * is allocated here, which is passed to `writeDiskBlock` method, which takes care
 * of freeing the pointer. This method should not free the memory again.
 */
void initializeDiskBlocks(size_t freeBlockNum, size_t startPos) {
	size_t blockMemSize = sizeof(disk_block);
	size_t freeBlocksPerBlock = blockSize / blockAddressSize;

	disk_block *metaBlock;
	size_t pointerBlockNumber = freeBlockNum;

	while (pointerBlockNumber < numOfBlocks) {
		metaBlock = (disk_block *)malloc(blockMemSize);
		metaBlock = makeOneBlock(metaBlock, pointerBlockNumber);
		
		writeDiskBlock(pointerBlockNumber, metaBlock);
		pointerBlockNumber += freeBlocksPerBlock;
	}
}

