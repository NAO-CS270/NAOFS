#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include "mandsk/params.h"
#include "dsk/mdisk.h"
#include "inode/iNode.h"
#include "inode/inCoreiNode.h"

#include <stdbool.h>

struct directoryEntry {
	char name[FILENAME_SIZE];
	ssize_t iNodeNum;
};
typedef struct directoryEntry directoryEntry;

struct directoryTable {
	directoryEntry entries[DIRECTORY_ENTRIES_IN_BLOCK];
};
typedef struct directoryTable directoryTable;

extern directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock);
extern int searchINodeDirectoryEntries(inCoreiNode *iNodePtr, char *entryName, size_t offset, directoryEntry *entryBuffer, size_t numOfEntries, bool forUnlink);

extern int getAndUpdateDirectoryTable(inCoreiNode* inode, size_t newInodeNumber, char* filename);
extern int updateNewDirMetaData(inCoreiNode* inode, size_t parentInodeNumber);
extern void removeDirectoryEntry(size_t blockNum, size_t blockOffset, const char* filename, size_t iNodeNum);

extern directoryTable* getDirectoryEntries(inCoreiNode* inode);

extern void getLastEntry(inCoreiNode *iNodePtr, directoryEntry* entry);

/*
union directoryData {
	disk_block dataBlock;
	directoryTable data;
};
typedef union directoryData directoryData;
*/

#endif

