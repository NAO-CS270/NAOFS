#ifndef _INODE_FETCHER_H
#define _INODE_FETCHER_H

#include <stdio.h>
#include <stdlib.h>

#include "mkfs/metaBlocks.h"
#include "mandsk/params.h"
#include "dsk/blkfetch.h"
#include "dsk/mdisk.h"
#include "inode/iNode.h"
#include "inode/inCoreiNode.h"
#include "mkfs/diskParams.h"

extern size_t searchINodes(size_t startINodeNum, iNodeListBlock *iNodeNums);
extern void markINodeFree(size_t iNodeNum, iNodeType nodeType);
extern void getDiskInode(size_t iNodeNum, iNode* inode);
extern void writeDiskInode(size_t iNodeNum, iNode* inode);

#endif
