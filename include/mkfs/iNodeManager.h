#ifndef _INODE_FETCHER_H
#define _INODE_FETCHER_H

#include "mkfs/metaBlocks.h"
#include "inode/iNode.h"

#include <stddef.h>

extern size_t searchINodes(size_t startINodeNum, iNodeListBlock *iNodeNums);
extern void updateINodeData(size_t iNodeNum, iNodeType iType, iNodeMode iMode, size_t o_uid, size_t g_uid);
extern int getDiskInode(size_t iNodeNum, iNode* inode);
extern int writeDiskInode(size_t iNodeNum, iNode* inode);

#endif

