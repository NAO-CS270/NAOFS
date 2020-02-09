#ifndef _FREE_BLOCK_LIST_H
#define _FREE_BLOCK_LIST_H

/* Takes in pointer to a block and a block number. Leaving space for one block number,
 * fills the block with block numbers sequentially until the end. The next node number
 * is filled at the start of this block, indicating the next index to find free blocks.
 * This node number is also returned.
 */
extern disk_block *makeOneBlock(disk_block *blockPtr, size_t startNode);

#endif
