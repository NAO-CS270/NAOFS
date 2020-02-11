#ifndef FILETABLES_H
#define FILETABLES_H

#include <stdlib.h>
#include "./inode/inCoreiNode.h"

typedef  struct fileTable {
    size_t fd;
    inCoreiNode *inode;
    size_t mode;
    size_t offset;
}FileTable;

FileTable file_descriptor_table[MAX_FD];

extern size_t createFileDescriptorEntry(inCoreiNode *inode, size_t mode);
extern void initFileTableEntries();

typedef struct fileTable fileTable;
#endif //FILETABLES_H