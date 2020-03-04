#ifndef GLOBALFILETABLE_H
#define GLOBALFILETABLE_H

#include "inode/inCoreiNode.h"

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

struct fileTableEntry {
    inCoreiNode *inode;
    pid_t pid;
    int flags;
    size_t offset;
    bool validEntry;
};
typedef struct fileTableEntry fileTableEntry;

extern void initGlobalFileTable();
extern int createAndGetFileDescriptor(pid_t pid, inCoreiNode *inode, int flags, size_t offset);
extern fileTableEntry *getFileDescriptor(pid_t pid, int fd, int *error);

#endif //GLOBALFILETABLE_H
