#include "diskParams.h"
#include "mandsk/params.h"
#include "dsk/blkfetch.h"
#include "dsk/mdisk.h"
#include "inode/iNode.h"

static const size_t iNodesInABlock = BLOCK_SIZE/INODE_SIZE;
static const size_t iNodeNumsInABlock = BLOCK_SIZE/INODE_ADDRESS_SIZE;
static const size_t endOfINodeBlocks = INODE_BLOCKS_HEAD + ((INODE_SIZE * NUM_OF_INODES) / BLOCK_SIZE);

size_t getReturnValue(iNodeListBlock *iNodeNums, size_t freeINodeCounter) {
	if (freeINodeCounter == 0) {
		// TODO - throw an error. This means system is out of iNodes.
		return 0;
	}
	if (freeINodeCounter < iNodeNumsInABlock) {
		// TODO - this means next search would be in vain.
		return 0;
	}

	return *((iNodeNums->iNodesNos) + (freeINodeCounter - 1));
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

	disk_block *iNodesData;
	iNodesBlock *iNodesList = (iNodesBlock *)malloc(sizeof(iNodesBlock));

	size_t blockCounter = (startINodeNum/iNodesInABlock) + INODE_BLOCKS_HEAD;
	size_t freeINodeCounter = 0;

	while (freeINodeCounter < iNodeNumsInABlock) {
		if (blockCounter >= endOfINodeBlocks) {
			break;
		}
		iNodesData = getDiskBlock(blockCounter);
		makeINodesBlock(iNodesData, iNodesList);

		freeINodeCounter += getFromBlock(iNodesList, (iNodeNums->iNodeNos) + freeINodeCounter);
		blockCounter++;
	}
	free(iNodesList);
	return getReturnValue(iNodeNums, freeINodeCounter);
}

/* Modifies iNode with iNode number `iNodeNum` as the boolean `toSetType` and writes it to disk. */
void markINodeFree(size_t iNodeNum, bool toSetType) {
	if (iNodeNum >= NUM_OF_INODES) {
		// TODO - Throw an error
		return ;
	}
	size_t blockNum = (iNodeNum/iNodesInABlock) + INODE_BLOCKS_HEAD;

	disk_block *iNodesData = getDiskBlock(blockNum);
	iNodesBlock *iNodesList = (iNodesBlock *)malloc(sizeof(iNodesBlock));
	makeINodesBlock(iNodesData, iNodesList);
	
	iNode *theINode = blockOfINodes->iNodesList;
	size_t iNodeIterator = 0;

	while (iNodeIterator < iNodesInABlock) {
		if ((theINode->inode_number) == iNodeNum) {
			(theINode->type) = toSetType;
			break;
		}
		theINode++;
		iNodeIterator++;
	}
	writeINodesBlock(iNodesList, iNodesData);
	writeDiskBlock(iNodesData);

	free(iNodesList);
}

