#include <stddef.h>
#include <stdlib.h>

#include "inode/iNode.h"
#include "dsk/blkfetch.h"
#include "mandsk/params.h"
#include "freeBlockList.h"

static const size_t blockAddressSize = BLOCK_ADDRESS_SIZE;
static const size_t iNodeAddressSize = INODE_ADDRESS_SIZE;

static const size_t blockSize = BLOCK_SIZE;
static const size_t numOfBlocks = NUM_OF_BLOCKS;
static const size_t iNodeSize = INODE_SIZE;
static const size_t numOfINodes = NUM_OF_INODES;

static size_t numOfINodeBlocks = 0;

size_t writeINodeListToDisk(size_t freeBlockNum, size_t *iNodeList, size_t iNodeListSize) {
	size_t iNodeCounter = 0;
	while (iNodeCounter < iNodeListSize) {
		*(iNodeList + iNodeCounter) = iNodeCounter;
		iNodeCounter++;
	}

	disk_block *metaBlock = (disk_block *)malloc(blockSize);
	size_t rememberedINode = makeINodeListBlock(metaBlock, iNodeList, iNodeListSize);
	writeDiskBlock(freeBlockNum, metaBlock);

	return numOfINodeBlocks;
}

size_t getINodeListSize() {
	size_t iNodeAddressesPerBlock = blockSize / iNodeAddressSize;
	if (iNodeAddressesPerBlock > numOfINodes) {
		iNodeAddressesPerBlock = numOfINodes;
	}
	return iNodeAddressesPerBlock;
}

/* Initializes `numOfINodes` empty iNodes and saves them to disk. */
void createINodes(size_t startPos) {
	disk_block *metaBlock;
	size_t iNodeCounter = 0;

	numOfINodeBlocks = (numOfINodes * iNodeSize) / blockSize;
	size_t blockCounter = startPos;
	size_t endOfINodeBlocks = blockCounter + numOfINodeBlocks;

	while (blockCounter < endOfINodeBlocks) {
		metaBlock = (disk_block *)malloc(blockSize);
		iNodeCounter = populateINodesIn(metaBlock, iNodeCounter);

		writeDiskBlock(blockCounter, metaBlock);
		blockCounter++;
	}
}

/* Initializes `numOfINodes` empty iNodes and saves them to disk. Then, it forms
 * the list of iNodes in the superblock free iNode list.
 */
size_t initializeINodeData(size_t freeBlockNum, size_t startPos) {
	createINodes(startPos);

	// Keeping this an array of `size_t`, but storing in disk must be done in units of `iNodeAddressSize`.
	size_t iNodeListSize = getINodeListSize();
	size_t *iNodeList = (size_t *)malloc(iNodeListSize * sizeof(size_t));

	size_t iNodeToRemember = writeINodeListToDisk(freeBlockNum, iNodeList, iNodeListSize);
	return iNodeToRemember;
}

/* Sets up the free block list structure, assuming that the free data blocks start
 * at `startPos`, and the super block free list block is at `freeBlockNum`. A block
 * is allocated here, which is passed to `writeDiskBlock` method, which takes care
 * of freeing the pointer. This method should not free the memory again.
 *
 * TODO - Consider zero-ing all the blocks
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

/* Only this method must be exposed to be called as MKFS API. */
void makeFileSystem() {
	const size_t metaBlocks = 4;
	const size_t bootBlock = 0;
	const size_t superBlock = 1;
	const size_t freeListBlock = 2;
	const size_t iNodeListBlock = 3;
	const size_t iNodeBlocksHead = 4;

	size_t iNodeToRemember = initializeINodeData(iNodeListBlock, iNodeBlocksHead);
	// `numOfINodeBlocks` is global static, and is expected to be set appropriately before this.
	initializeDiskBlocks(freeListBlock, 4 + numOfINodeBlocks);
}

