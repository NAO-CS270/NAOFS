#ifndef _INODE_FETCHER_H
#define _INODE_FETCHER_H

extern size_t searchINodes(size_t startINodeNum, iNodeListBlock *iNodeNums);
extern void markINodeFree(size_t iNodeNum, bool isFree);
extern void getDiskInode(inCoreiNode* inode);
extern void writeDiskInode(inCoreiNode* inode);

#endif
