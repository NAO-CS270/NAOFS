#include "fdTable/globalFileTable.h"
#include "utils/utils.h"

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

static fileTableEntry fileTable[MAX_FILE_DESCRIPTORS];

/**
 * Initialises the file table
 */
void initGlobalFileTable() {
    int counter = 0;
    while (counter < MAX_FILE_DESCRIPTORS) {
        fileTable[counter].validEntry = false;
        counter++;
    }
}

/**
 * File Descriptor Table entry constructor
 */
void initFdTableEntry (fileTableEntry* entry, inCoreiNode *inode, pid_t pid, int flags, size_t offset) {
    entry->inode = inode;
    entry->pid = pid;
    entry->flags = flags;
    entry->offset = offset;
    entry->validEntry = true;
    printf("INITIALIZED FD TABLE ENTRY TO TRUE! \n");
}

/**
 * Adds an entry to the file descriptor table
 */
int createAndGetFileDescriptor (pid_t pid, inCoreiNode *inode, int flags, size_t offset) {
    int counter = 0;

    while (counter < MAX_FILE_DESCRIPTORS) {
        if (!fileTable[counter].validEntry) {
            initFdTableEntry(fileTable + counter, inode, pid, flags, offset);
            return counter;
        }
        counter++;
    }

    return -ENFILE;
}

/**
 * Gets a file descriptor table entry based on the file descriptor and pid
 */
fileTableEntry *getFileDescriptor(pid_t pid, int fd, int *error) {
    printf("fd list: \n");
    printf("getFileDescriptor: fd: %d\n", fd);
    if (fd >= MAX_FILE_DESCRIPTORS) {
        *error = -EBADF;
        return NULL;
    }

    // int counter = 0;
    // while (counter < MAX_FILE_DESCRIPTORS) {
    //     printf("validity of fdTable[Counter: %d]: %d ", counter, fileTable[counter].validEntry);
    //     counter++;
    // }
    // printf("\n");

    fileTableEntry *entry = fileTable + fd;
    if (!entry->validEntry) {
        *error = -EBADF;
        printf("returning NULL -EBADF\n");
        return NULL;
    }
    // if (entry->pid != pid) {
    //     *error = -EACCES;
    //     return NULL;
    // }

    *error = 0;
    return entry;
}
