#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include "mandsk/params.h"
#include "dsk/mdisk.h"
#include "inode/iNode.h"
#include "inode/inCoreiNode.h"

struct directoryEntry {
	char name[FILENAME_SIZE];
	size_t iNodeNum;
};
typedef struct directoryEntry directoryEntry;

struct directoryTable {
	directoryEntry entries[DIRECTORY_ENTRIES_IN_BLOCK];
};
typedef struct directoryTable directoryTable;

extern directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock);
extern int searchINodeDirectoryEntries(inCoreiNode *iNodePtr, char *entryName, size_t offset, directoryEntry *entryBuffer, size_t numOfEntries);

extern int getAndUpdateDirectoryTable(inCoreiNode* inode, size_t newInodeNumber, char* filename);
extern int updateNewDirMetaData(inCoreiNode* inode, size_t parentInodeNumber);

extern directoryTable* getDirectoryEntries(inCoreiNode* inode);

/*
union directoryData {
	disk_block dataBlock;
	directoryTable data;
};
typedef union directoryData directoryData;
*/

#endif

