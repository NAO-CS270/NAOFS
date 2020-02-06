#ifndef FILETABLES_H
#define FILETABLES_H
#include "inode/inCoreiNode.h"
#include "mandsk/params.h"

typedef  struct fileTable {
    size_t fd;
    inCoreiNode *inode;
    size_t mode;
}FileTable;

FileTable file_descriptor_table[MAX_FD];

extern size_t putFileDescriptorEntry(inCoreiNode *inode, size_t mode);

typedef fileTable fileTable;
#endif //PROJECT1_FILETABLES_H
