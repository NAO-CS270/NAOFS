#ifndef _INODE_FETCHER_H
#define _INODE_FETCHER_H

#include "mkfs/metaBlocks.h"
#include "inode/iNode.h"

#include <stddef.h>
#include <sys/types.h>

extern size_t searchINodes(size_t startINodeNum, iNodeListBlock *iNodeNums);
extern void updateINodeData(size_t iNodeNum, iNodeType iType, mode_t file_mode, uid_t o_uid, gid_t g_uid, size_t size, size_t linksCount);
extern int getDiskInode(size_t iNodeNum, iNode* inode);
extern int writeDiskInode(size_t iNodeNum, iNode* inode);

#endif

