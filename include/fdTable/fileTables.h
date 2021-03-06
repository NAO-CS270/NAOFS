// #ifndef FILETABLES_H
// #define FILETABLES_H

// #include "inode/inCoreiNode.h"

// #include <stdbool.h>
// #include <stdlib.h>
// #include <sys/types.h>

// struct fileTableEntry {
//     inCoreiNode *inode;
//     int flags;
//     size_t offset;
//     bool validEntry;
// };
// typedef struct fileTableEntry fileTableEntry;

// struct fileDescriptorNode {
//     fileTableEntry fdTable[MAX_FD];
//     pid_t pid;

//     struct fileDescriptorNode *next;
//     struct fileDescriptorNode *prev;
// };
// typedef struct fileDescriptorNode fdNode;

// extern int createAndGetFileDescriptor(pid_t pid, inCoreiNode *inode, int flags, size_t offset);
// extern fileTableEntry *getFileDescriptor(pid_t pid, int fd, int *error);
// void removeNodeIfEmpty(pid_t pid);

// #endif //FILETABLES_H
