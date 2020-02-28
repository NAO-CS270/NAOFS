#include "fdTable/fileTables.h"
#include "utils/utils.h"

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

static fdNode *fdListHead = NULL;

/**
 * FD Node constructor
 */
void initFdNode (fdNode *node, pid_t pid) {
    int counter = 0;
    while (counter < MAX_FD) {
        (node->fdTable)[counter].validEntry = false;
        counter++;
    }

    node->pid = pid;
    node->prev = NULL;
    node->next = NULL;
}

/**
 * checks if a node corresponding to a pid already exists, if not, creates a new node and
 * returns it
 */
fdNode *getFdNode(pid_t pid, bool search) {
    fdNode* tempHead = fdListHead;
    
    while(tempHead != NULL) {
        if(tempHead->pid == pid) {
            debug_print("found fd list node for pid: %d\n", pid);
            return tempHead;
        }
        tempHead = tempHead->next;
    }

    if (search) {
        debug_print("FD list node not found for pid: %d\n", pid);
        return NULL;
    }
    
    fdNode *newFdNode = (fdNode*)malloc(sizeof(fdNode));
    initFdNode(newFdNode, pid);
    newFdNode->prev = tempHead;
    newFdNode->next = NULL;

    if (NULL == fdListHead) {
        debug_print("fdList empty, can't find a node for pid: %d\n", pid);
        fdListHead = newFdNode;
    }

    debug_print("inserted new node in the fd list for pid: %d\n", pid);
    return newFdNode;
}

/**
 * File Descriptor Table entry constructor
 */
void initFdTableEntry (fileTableEntry* entry, inCoreiNode *inode, int flags, size_t offset) {
    entry->inode = inode;
    entry->flags = flags;
    entry->offset = offset;
    entry->validEntry = true;
}

/**
 * Adds an entry to the file descriptor table
 */
int createFdEntry (fileTableEntry *fdTable, inCoreiNode *inode, int flags, size_t offset) {
    int counter = 0;

    while (counter < MAX_FD) {
        if (!fdTable[counter].validEntry) {
            initFdTableEntry(fdTable + counter, inode, flags, offset);
            return counter;
        }
        counter++;
    }

    return -ENFILE;
}

/**
 * Adds an entry to the file descriptor table of process identified by pid
 */
int createAndGetFileDescriptor(pid_t pid, inCoreiNode *inode, int flags, size_t offset) {
    fdNode* fdNode = getFdNode(pid, false);

    return createFdEntry(fdNode->fdTable, inode, flags, offset);
}

/**
 * Gets an file descriptor table entry based on the file descriptor and pid
 */
fileTableEntry *getFileDescriptor(pid_t pid, int fd, int *error) {
    fdNode* fdNode = getFdNode(pid, true);
    if (NULL == fdNode || fd >= MAX_FD) {
        *error = -EBADF;
        return NULL;
    }

    fileTableEntry *entry = (fdNode->fdTable) + fd;
    if (!entry->validEntry) {
        *error = -EBADFD;
        return NULL;
    }

    *error = 0;
    return entry;
}
