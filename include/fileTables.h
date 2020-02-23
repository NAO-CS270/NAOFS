#ifndef FILETABLES_H
#define FILETABLES_H

#include "inode/inCoreiNode.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct fileTableNode {
    inCoreiNode *inode;
    size_t mode;
    size_t offset;
    size_t count;

    struct fileTableNode *next;
    struct fileTableNode *prev;
}fileTableNode;

fileTableNode* fileTableNodeHead;

typedef struct fdTableEntry {
    fileTableNode *fileTableNode;
}fdTableEntry;

typedef struct fdTable {
    fdTableEntry fdTableEntry[MAX_FD];
}fdTable;

typedef struct fileDescriptorNode {
    fdTable *fdTable;
    size_t pid;
    size_t nextEntryIndex;
    size_t lastInsertedIndex;
    bool validEntry;
    struct fileDescriptorNode *next;
    struct fileDescriptorNode *prev;
}fdNode;

fdNode *fdListHead;
//// REMOVE THIS STRUCT
//typedef  struct fileTable {
//
//}FileTable;
//
//FileTable file_descriptor_table[MAX_FD];

extern size_t createFileDescriptorEntry(inCoreiNode *inode, size_t mode);
extern void initFileTableEntries();
extern int getFileDescriptor(size_t pid, size_t mode, inCoreiNode* inode);
typedef struct fileTable fileTable;
#endif //FILETABLES_H
