//#include "mandsk/params.h"
#include "inode/iNode.h"

size_t INODE_SIZE;
size_t INODES_PER_BLOCK;
size_t NUM_OF_INODE_BLOCKS;

void initializeINodeParams() {
    size_t iNodeStructSize = sizeof(iNode);

    size_t iNodeSize = 1;

    while(iNodeStructSize != 0) {
        iNodeSize *= 2;
        iNodeStructSize /= 2;
    }

    INODE_SIZE = iNodeSize;
    INODES_PER_BLOCK = BLOCK_SIZE/INODE_SIZE;
	NUM_OF_INODE_BLOCKS = INODE_SIZE * NUM_OF_INODES / BLOCK_SIZE;
}

