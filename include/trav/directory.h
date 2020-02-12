#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include "mandsk/params.h"
#include "dsk/mdisk.h"
#include "inode/iNode.h"
#include "inode/inCoreiNode.h"

struct nameINodePair {
	char name[FILENAME_SIZE];
	size_t iNodeNum;
};
typedef struct nameINodePair nameINodePair;

struct directoryTable {
	nameINodePair entries[DIRECTORY_ENTRIES_IN_BLOCK];
};
typedef struct directoryTable directoryTable;

extern directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock);
extern size_t findINodeInDirectory(inCoreiNode *iNodePtr, char *entryName);

extern void getAndUpdateDirectoryTable(inCoreiNode* inode, size_t newInodeNumber, char* filename);
void updateNewDirMetaData(inCoreiNode* inode, size_t newInodeNumber, size_t parentInodeNumber);

extern directoryTable* getDirectoryEntries(inCoreiNode* inode);

/*
union directoryData {
	disk_block dataBlock;
	directoryTable data;
};
typedef union directoryData directoryData;
*/

#endif
