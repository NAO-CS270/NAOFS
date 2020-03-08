#include "mkfs/iNodeManager.h"
#include "inode/iNode.h"
#include "mandsk/params.h"
#include "mkfs/diskParams.h"
#include "dsk/blkfetch.h"

#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define iNodeNumsInABlock (BLOCK_SIZE/INODE_ADDRESS_SIZE)
#define endOfINodeBlocks (INODE_BLOCKS_HEAD + ((INODE_SIZE * NUM_OF_INODES) / BLOCK_SIZE))

enum iNodeAccessType {
	INODE_READ = 0,
	INODE_WRITE = 1,
};
typedef enum iNodeAccessType iNodeAccessType;

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

	while (iNodeIterator < INODES_PER_BLOCK) {
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

	iNode *listOfINodes = (iNode *)malloc(INODES_PER_BLOCK * sizeof(iNode));
	iNodesBlock *iNodesList = (iNodesBlock *)malloc(sizeof(iNodesBlock));
	iNodesList->iNodesList = listOfINodes;

	size_t blockCounter = (startINodeNum/INODES_PER_BLOCK) + INODE_BLOCKS_HEAD;
	size_t freeINodeCounter = 0;

	while (freeINodeCounter < iNodeNumsInABlock) {
        disk_block *iNodesData = (disk_block*)malloc(sizeof(disk_block));
		if (blockCounter >= endOfINodeBlocks) {
			break;
		}
		getDiskBlock(blockCounter, iNodesData);
		makeINodesBlock(iNodesData, iNodesList);

		freeINodeCounter += getFromBlock(iNodesList, (iNodeNums->iNodeNos) + freeINodeCounter);
		blockCounter++;
		// TODO: free iNodesData?
	}
	free(listOfINodes);
	free(iNodesList);
	return getReturnValue(iNodeNums, freeINodeCounter);
}

int accessINodeAndItsBlock(size_t iNodeNum, iNode *inode, disk_block *blockPtr, iNodeAccessType accessType) {
	if (iNodeNum >= NUM_OF_INODES) {
		return -1;
	}

	disk_block *metaBlock = blockPtr;
	if (blockPtr == NULL) {
		metaBlock = (disk_block *)malloc(BLOCK_SIZE);
	}

	size_t blockNum = (iNodeNum/INODES_PER_BLOCK) + INODE_BLOCKS_HEAD;
	getDiskBlock(blockNum, metaBlock);
	unsigned char *ptrIntoBlock = metaBlock->data;

	size_t index = iNodeNum % INODES_PER_BLOCK;
	if (accessType == INODE_READ) {
		memcpy(inode, ptrIntoBlock + (index * INODE_SIZE), sizeof(iNode));
	}
	else {
		memset(ptrIntoBlock + (index * INODE_SIZE), 0, INODE_SIZE);
		memcpy(ptrIntoBlock + (index * INODE_SIZE), inode, sizeof(iNode));
		writeDiskBlock(blockNum, metaBlock);
	}

	if (blockPtr == NULL) {
		free(metaBlock);
	}

	return 0;
}

/* Modifies iNode with iNode number `iNodeNum` to given parameters.*/
void updateINodeData(size_t iNodeNum, iNodeType iType, mode_t mode, uid_t o_uid, gid_t g_uid, size_t size, size_t linksCount) {
	iNode *inode = (iNode *)malloc(sizeof(iNode));

	accessINodeAndItsBlock(iNodeNum, inode, NULL, INODE_READ);
	inode->status_change = time(NULL);
	inode->type = iType;
	inode->file_mode = mode;
	inode->owner_uid = o_uid;
	inode->group_uid = g_uid;
	inode->size = size;
	inode->linksCount = linksCount;

	accessINodeAndItsBlock(iNodeNum, inode, NULL, INODE_WRITE);
	free(inode);
}

int getDiskInode(size_t iNodeNum, iNode* inode) {
	return accessINodeAndItsBlock(iNodeNum, inode, NULL, INODE_READ);
}

int writeDiskInode(size_t iNodeNum, iNode* inode) {
	return accessINodeAndItsBlock(iNodeNum, inode, NULL, INODE_WRITE);
}

