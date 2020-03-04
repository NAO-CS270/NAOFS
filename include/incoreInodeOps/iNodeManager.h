#include "inode/inCoreiNode.h"

#include <stdlib.h>

enum indirectionLevel {
	DIRECT = 0,
	SINGLE_INDIRECT = 1,
	DOUBLE_INDIRECT = 2,
	TRIPLE_INDIRECT = 3
};
typedef enum indirectionLevel indirectionLevel;

struct blkTreeOffset {
	int offsets[4];
	indirectionLevel offsetIndirection;
};
typedef struct blkTreeOffset blkTreeOffset;

/**
 *	Updates in-core inode data blocks list with the newly allocated block
 *	When this function is called the size should point at the head of new block
 * 	=> (size % BLOCK_SIZE) == 0
 * 	inode: in-core copy of inode
 *	blockNumToAdd: block to address to add
 */
extern void insertDataBlockInINode (inCoreiNode* inode, size_t blockNumToAdd);

/**
 *	Calculates different offsets based on the given file offset
 *	blkOffset: the block tree offset struct
 *	offset: offset of the file
 */
extern void calculateOffset(size_t offset, blkTreeOffset* blkOffset);

extern void updateINodeMetadata(inCoreiNode *iNode, int sizeDifference, size_t linkCount);
