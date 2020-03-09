#include "mkfs/diskParams.h"
#include "inode/iNode.h"
#include "dsk/blkfetch.h"
#include "mandsk/params.h"
#include "mkfs/freeBlockList.h"
#include "mkfs/metaBlocks.h"
#include "mkfs/alloc.h"
#include "mkfs/iNodeManager.h"
#include "mkfs/ialloc.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

void assignRootData() {
	size_t rootINodeNum = 0;
	iNode *rootINode = (iNode *)malloc(sizeof(iNode));
	getDiskInode(rootINodeNum, rootINode);
	size_t blockNum = blockAlloc();
	rootINode->type = T_DIRECTORY;
	rootINode->file_mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IWGRP | S_IROTH | S_IXOTH | S_IWOTH;
	rootINode->dataBlockNums[0] = blockNum;
	writeDiskInode(rootINodeNum, rootINode);
	free(rootINode);
}

size_t writeINodeListToDisk(size_t freeBlockNum, size_t iNodeListSize) {
	assignRootData();

	size_t iNodeCounter = 0;
	size_t *iNodeList = (size_t *)malloc(iNodeListSize * sizeof(size_t));
	while (iNodeCounter < iNodeListSize) {
		*(iNodeList + iNodeCounter) = iNodeCounter + 1;
		iNodeCounter++;
	}

	cacheNode *diskBlockNode = getDiskBlockNode(freeBlockNum, 0);

	size_t rememberedINode = initINodeListBlock(diskBlockNode->dataBlock, iNodeList, iNodeListSize);

	diskBlockNode->header->delayedWrite = true;
	writeDiskBlockNode(diskBlockNode);

	free(iNodeList);
	return rememberedINode;
}

size_t getINodeListSize() {
	size_t iNodeAddressesPerBlock = BLOCK_SIZE / INODE_ADDRESS_SIZE;
	if (iNodeAddressesPerBlock > NUM_OF_INODES) {
		iNodeAddressesPerBlock = NUM_OF_INODES;
	}
	return iNodeAddressesPerBlock;
}

/* Initializes `NUM_OF_INODES` empty iNodes and saves them to disk. */
void createINodes(size_t startPos) {
	cacheNode *diskBlockNode;
	size_t iNodeCounter = 0;

	size_t blockCounter = startPos;
	size_t endOfINodeBlocks = blockCounter + NUM_OF_INODE_BLOCKS;

	while (blockCounter < endOfINodeBlocks) {
		diskBlockNode = getDiskBlockNode(blockCounter, 0);

		iNodeCounter = populateINodesIn(diskBlockNode->dataBlock, iNodeCounter);

		diskBlockNode->header->delayedWrite = true;
		writeDiskBlockNode(diskBlockNode);

		blockCounter++;
	}
}

/* Initializes `NUM_OF_INODES` empty iNodes and saves them to disk. Then, it forms
 * the list of iNodes in the superblock free iNode list.
 */
size_t initializeINodeData(size_t freeBlockNum, size_t startPos) {
	createINodes(startPos);

	// Keeping this an array of `size_t`, but storing in disk must be done in units of `INODE_ADDRESS_SIZE`.
	size_t iNodeListSize = getINodeListSize();

	size_t iNodeToRemember = writeINodeListToDisk(freeBlockNum, iNodeListSize);
	return iNodeToRemember;
}

/* Sets up the free block list structure, assuming that the free data blocks start
 * at `startPos`, and the super block free list block is at `freeBlockNum`.
 *
 * TODO - Consider zero-ing all the blocks
 */
void initializeDiskBlocks(size_t freeBlockNum, size_t startPos) {
	size_t freeBlocksPerBlock = BLOCK_SIZE / BLOCK_ADDRESS_SIZE;

	cacheNode *diskBlockNode;

	size_t pointerBlockNumber = freeBlockNum;
	size_t nextFreeListNumber;
	while (pointerBlockNumber < NUM_OF_BLOCKS) {
		diskBlockNode = getDiskBlockNode(pointerBlockNumber, 0);

		nextFreeListNumber = makeOneBlock(diskBlockNode->dataBlock, startPos);

		diskBlockNode->header->delayedWrite = true;
		writeDiskBlockNode(diskBlockNode);

		if (nextFreeListNumber == 0) {
			break;
		}
		pointerBlockNumber = nextFreeListNumber;
		startPos += freeBlocksPerBlock;
	}
}

/* Only this method must be exposed to be called as MKFS API. */
void makeFileSystem() {
	initializeINodeParams();

	size_t iNodeToRemember = initializeINodeData(INODE_LIST_BLOCK, INODE_BLOCKS_HEAD);
	
	initializeDiskBlocks(FREE_LIST_BLOCK, 4 + NUM_OF_INODE_BLOCKS);

	cacheNode *diskBlockNode = getDiskBlockNode(SUPER_BLOCK, 0);
	superBlock *theSuperBlock = (superBlock *)malloc(sizeof(superBlock));

	(theSuperBlock->rememberedINode) = iNodeToRemember;
	writeSuperBlock(theSuperBlock, diskBlockNode->dataBlock);

	writeDiskBlockNode(diskBlockNode);

	free(theSuperBlock);
}

