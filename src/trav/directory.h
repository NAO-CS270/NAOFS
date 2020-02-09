#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include "mandsk/params.h"
#include "dsk/mdisk.h"

#define ENTRIES_PER_BLOCK (BLOCK_SIZE/sizeof(nameINodePair))

struct nameINodePair {
	char name[FILENAME_SIZE];
	size_t iNodeNum;
};
typedef struct nameINodePair nameINodePair;

struct directoryTable {
	nameINodePair entries[ENTRIES_PER_BLOCK];
};
typedef struct directoryTable directoryTable;

extern directoryTable *makeDirectoryTable(disk_block *blockPtr, directoryTable *theBlock);
extern size_t findINodeInDirectory(iNode *iNodePtr, char *entryName);
/*
union directoryData {
	disk_block dataBlock;
	directoryTable data;
};
typedef union directoryData directoryData;
*/

#endif
