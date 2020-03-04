// #include "fdTable/fileTables.h"
// #include "utils/utils.h"

// #include <errno.h>
// #include <stdlib.h>
// #include <stdbool.h>

// static fdNode *fdListHead = NULL;

// /**
//  * FD Node constructor
//  */
// void initFdNode (fdNode *node, pid_t pid) {
//     int counter = 0;
//     while (counter < MAX_FD) {
//         (node->fdTable)[counter].validEntry = false;
//         counter++;
//     }
//     printf("PID in initFdNode: %d\n", node->pid);
//     node->pid = pid;
//     node->prev = NULL;
//     node->next = NULL;
// }

// void printFdList() {
//     fdNode* tempHead = fdListHead;
//     while(tempHead != NULL) {
//         printf("pid: %d\n", tempHead->pid);
//         tempHead = tempHead->next;
//     }
// }

// /**
//  * checks if a node corresponding to a pid already exists, if not, creates a new node and
//  * returns it
//  */
// fdNode *getFdNode(pid_t pid, bool search) {
//     fdNode* tempHead = fdListHead;
//     fdNode* prevNode = fdListHead;
//     while(tempHead != NULL) {
//         if(tempHead->pid == pid) {
//             debug_print("found fd list node for pid: %d\n", pid);
//             return tempHead;
//         }
//         tempHead = tempHead->next;
//         if (NULL != tempHead) {
//             prevNode = tempHead;
//         }
//     }

//     if (search) {
//         debug_print("FD list node not found for pid: %d\n", pid);
//         return NULL;
//     }
    
//     fdNode *newFdNode = (fdNode*)malloc(sizeof(fdNode));
//     initFdNode(newFdNode, pid);
//     newFdNode->prev = prevNode;
//     newFdNode->next = NULL;
//     if (NULL != prevNode) {
//         prevNode->next = newFdNode;
//     }
//     if (NULL == fdListHead) {
//         debug_print("fdList empty, can't find a node for pid: %d\n", pid);
//         fdListHead = newFdNode;
//     }

//     debug_print("inserted new node in the fd list for pid: %d\n", pid);
//     return newFdNode;
// }

// /**
//  * File Descriptor Table entry constructor
//  */
// void initFdTableEntry (fileTableEntry* entry, inCoreiNode *inode, int flags, size_t offset) {
//     entry->inode = inode;
//     entry->flags = flags;
//     entry->offset = offset;
//     entry->validEntry = true;
//     printf("INITIALIZED FD TABLE ENTRY TO TRUE! \n");
// }

// /**
//  * Adds an entry to the file descriptor table
//  */
// int createFdEntry (fileTableEntry *fdTable, inCoreiNode *inode, int flags, size_t offset) {
//     int counter = 0;

//     while (counter < MAX_FD) {
//         if (!fdTable[counter].validEntry) {
//             initFdTableEntry(fdTable + counter, inode, flags, offset);
//             return counter;
//         }
//         counter++;
//     }

//     return -ENFILE;
// }

// /**
//  * Adds an entry to the file descriptor table of process identified by pid
//  */
// int createAndGetFileDescriptor(pid_t pid, inCoreiNode *inode, int flags, size_t offset) {
//     printf("Create and Get File Descriptor!! \n");
//     fdNode* fdNode = getFdNode(pid, false);
//     printf("fd list in createAndGetFileDescriptor: \n");
//     printFdList(); 
//     return createFdEntry(fdNode->fdTable, inode, flags, offset);
// }

// /**
//  * Gets an file descriptor table entry based on the file descriptor and pid
//  */
// fileTableEntry *getFileDescriptor(pid_t pid, int fd, int *error) {
//     printf("fd list: \n");
//     printFdList();
//     printf("getFileDescriptor: fd: %d\n", fd);
//     fdNode* fdNode = getFdNode(pid, true);
//     if (NULL == fdNode || fd >= MAX_FD) {
//         *error = -EBADF;
//         return NULL;
//     }
//     int counter = 0;
//     while (counter < MAX_FD) {
//         printf("validity of fdTable[Counter: %d]: %d", counter, fdNode->fdTable[counter].validEntry);
//         counter++;
//     }
//     fileTableEntry *entry = (fdNode->fdTable) + fd;
//     if (!entry->validEntry) {
//         *error = -EBADF;
//         printf("returning NULL -EBADF\n");
//         return NULL;
//     }

//     *error = 0;
//     return entry;
// }

// void removeNode(fdNode *node) {
//     if (node != fdListHead) {
//         node->prev->next = node->next;
//     }
//     if (NULL != node->next) {
//         node->next->prev = node->prev;
//     }
//     if (node == fdListHead) {
//         fdListHead = node->next;
//     }
// }

// void removeNodeIfEmpty(pid_t pid) {
//     fdNode* fdNode = getFdNode(pid, true);
//     if (NULL == fdNode) {
//         return;
//     }

//     int counter = 0;
//     bool empty = true;
//     while (counter < MAX_FD) {
//         if ((fdNode->fdTable)[counter].validEntry) {
//             empty = false;
//             break;
//         }
//         counter++;
//     }
//     if (empty) {
//         removeNode(fdNode);
//         free(fdNode);
//     }
// }
