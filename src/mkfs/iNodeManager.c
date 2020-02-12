#include "mkfs/iNodeManager.h"

#define iNodesInABlock BLOCK_SIZE/INODE_SIZE
#define iNodeNumsInABlock (BLOCK_SIZE/INODE_ADDRESS_SIZE)
#define endOfINodeBlocks (INODE_BLOCKS_HEAD + ((INODE_SIZE * NUM_OF_INODES) / BLOCK_SIZE))

size_t getReturnValue(iNodeListBlock *iNodeNums, size_t freeINodeCounter) {
	if (freeINodeCounter == 0) {
		// TODO - throw an error. This means system is out of iNodes.
		return 0;
	}
	if (freeINodeCounter < iNodeNumsInABlock) {
		// TODO - this means next search would be in vain.
		return 0;
	}

	return *((iNodeNums->iNodeNos) + (freeINodeCounter - 1));
}

size_t getFromBlock(iNodesBlock *blockOfINodes, size_t *nextAddrAt) {
	iNode *theINode = blockOfINodes->iNodesList;
	size_t collectedCount = 0;
	size_t iNodeIterator = 0;

	while (iNodeIterator < iNodesInABlock) {
		if (theINode->type == T_FREE) {
			*nextAddrAt = theINode->inode_number;
			nextAddrAt++;
			collectedCount++;
		}
		theINode++;
		iNodeIterator++;
	}
	
	return collectedCount;
}

/* Searches disk for free iNodes starting from iNode number `startINodeNum` and fills up the
 * buffer passed in `iNodeNums`.
 */
size_t searchINodes(size_t startINodeNum, iNodeListBlock *iNodeNums) {
	if (startINodeNum == 0) {
		// TODO - throw an error. This means system is out of iNodes.
		return 0;
	}

	iNodesBlock *iNodesList = (iNodesBlock *)malloc(sizeof(iNodesBlock));

	size_t blockCounter = (startINodeNum/iNodesInABlock) + INODE_BLOCKS_HEAD;
	size_t freeINodeCounter = 0;

	while (freeINodeCounter < iNodeNumsInABlock) {
        disk_block *iNodesData = (disk_block*)malloc(sizeof(disk_block));
		if (blockCounter >= endOfINodeBlocks) {
			break;
		}
		iNodesData = getDiskBlock(blockCounter, iNodesData);
		makeINodesBlock(iNodesData, iNodesList);

		freeINodeCounter += getFromBlock(iNodesList, (iNodeNums->iNodeNos) + freeINodeCounter);
		blockCounter++;
		// TODO: free iNodesData?
	}
	free(iNodesList);
	return getReturnValue(iNodeNums, freeINodeCounter);
}

/* Modifies iNode with iNode number `iNodeNum` as the boolean `toSetType` and writes it to disk. */
void markINodeFree(size_t iNodeNum, iNodeType nodeType) {
	if (iNodeNum >= NUM_OF_INODES) {
		// TODO - Throw an error
		return ;
	}
	size_t blockNum = (iNodeNum/iNodesInABlock) + INODE_BLOCKS_HEAD;

	disk_block *iNodesData = (disk_block*)malloc(sizeof(disk_block));
	iNodesData = getDiskBlock(blockNum, iNodesData);
	iNodesBlock *blockOfINodes = (iNodesBlock *)malloc(sizeof(iNodesBlock));
	makeINodesBlock(iNodesData, blockOfINodes);
	//TODO: Free iNotesData?
	iNode *theINode = blockOfINodes->iNodesList;
	size_t iNodeIterator = 0;

	while (iNodeIterator < iNodesInABlock) {
		if ((theINode->inode_number) == iNodeNum) {
			(theINode->type) = nodeType;
			break;
		}
		theINode++;
		iNodeIterator++;
	}
	writeINodesBlock(blockOfINodes, iNodesData);
	writeDiskBlock(blockNum, iNodesData);

	free(blockOfINodes);
}

void getDiskInode(size_t iNodeNum, iNode* inode) {
	if (iNodeNum >= NUM_OF_INODES) {
		// TODO - Throw an error
		return ;
	}

	size_t blockNum = (iNodeNum/iNodesInABlock) + INODE_BLOCKS_HEAD;
	disk_block* metaBlock = (disk_block*)malloc(BLOCK_SIZE);
	metaBlock = getDiskBlock(blockNum, metaBlock);

	iNodesBlock* iNodeBlk = (iNodesBlock*)malloc(sizeof(iNodesBlock));
	iNodeBlk = makeINodesBlock(metaBlock, iNodeBlk);

	iNode* iNodesList = iNodeBlk->iNodesList;
	size_t index = iNodeNum % iNodesInABlock;

	// the doubtful part, this or memcpy

	*inode = (iNodesList[index]);

	free(iNodeBlk);
	free(metaBlock);
}

void writeDiskInode(size_t iNodeNum, iNode* inode) {
	// TODO: get a lock before reading and release after writing
	if (iNodeNum >= NUM_OF_INODES) {
		// TODO - Throw an error
		return ;
	}

	size_t blockNum = (iNodeNum/iNodesInABlock) + INODE_BLOCKS_HEAD;
	disk_block* metaBlock = (disk_block*)malloc(BLOCK_SIZE);
	metaBlock = getDiskBlock(blockNum, metaBlock);

	iNodesBlock* iNodeBlk = (iNodesBlock*)malloc(sizeof(iNodesBlock));
	iNodeBlk = makeINodesBlock(metaBlock, iNodeBlk);

	iNode* iNodesList = iNodeBlk->iNodesList;
	size_t index = iNodeNum % iNodesInABlock;

	// the doubtful part, this or memcpy
	(iNodesList[index]) = *inode;

	writeINodesBlock(iNodeBlk, metaBlock);
	writeDiskBlock(blockNum, metaBlock);

	free(iNodeBlk);
	free(metaBlock);
}

