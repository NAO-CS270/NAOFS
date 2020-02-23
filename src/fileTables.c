#include "fileTables.h"
#include "utils/utils.h"

#include <stdbool.h>

/**
 * checks if a node corresponding to a pid already exists, if not, creates a new node and
 * returns it
 */
fdNode * getFdNode(size_t pid) {
    if(NULL == fdListHead) {
        debug_print("fdList empty, can't find a node for pid: %ld\n", pid);
        fdListHead = (fdNode *)malloc(sizeof(fdNode));
        fdListHead -> pid = pid;
        fdListHead -> validEntry = true;
        fdListHead -> prev = NULL;
        fdListHead -> next = NULL;
        fdListHead -> nextEntryIndex = 0;
        return fdListHead;
    } else {
        fdNode* tempHead = fdListHead;
        while(tempHead -> next != NULL) {
            if(tempHead -> pid == pid) {
                return tempHead;
            }
            tempHead = tempHead -> next;
        }
        if(tempHead -> pid == pid) {
            debug_print("found fd list node for pid: %ld\n", pid);
            return tempHead;
        }
        fdNode *newFdNode = (fdNode*)malloc(sizeof(fdNode));
        newFdNode -> pid = pid;
        newFdNode -> validEntry = true;
        newFdNode -> prev = tempHead;
        newFdNode -> next = NULL;
        newFdNode -> nextEntryIndex = 0;

        debug_print("inserted new node in the fd list for pid: %ld", pid);
        return newFdNode;
    }
}

/**
 * for a given process, fetches the fd table, and inserts a file table entry into the fd table
 */
fileTableNode* getFileTableEntryNode(size_t pid, fdNode* fdNode) {
    fileTableNode *node = (fileTableNode*)malloc(sizeof(fileTableNode));
    if(NULL == fileTableNodeHead) {
        node -> next = NULL;
        node -> prev = NULL;
        fileTableNodeHead = node;
    } else {
        fileTableNode* tempHead = fileTableNodeHead;
        while(tempHead -> next != NULL) {
            tempHead = tempHead -> next;
        }
        node -> prev = tempHead;
        tempHead -> next = node;
        node -> next = NULL;
    }
    if (fdNode -> nextEntryIndex == MAX_FD - 1) {
        debug_print("Maximum file descriptors open\n");
        return NULL;
    }
    fdNode ->fdTable[fdNode->nextEntryIndex].fdTableEntry-> fileTableNode = node;
    fdNode -> lastInsertedIndex = fdNode->nextEntryIndex;
    // TODO: not necessary that every time the next fd entry is a simple ++
    fdNode -> nextEntryIndex++;
    return node;
}

int getFileDescriptor(size_t pid, size_t mode, inCoreiNode* inode) {
    fdNode* fdNode = getFdNode(pid);
    fileTableNode *fileTableNode = getFileTableEntryNode(pid, fdNode);
    if(NULL == fileTableNode) {
        return -1;
    }
    fileTableNode -> inode = inode;
    fileTableNode -> mode = mode;

    return fdNode -> lastInsertedIndex;
}


//// putFileDescriptorEntry adds an entry to the file_descriptor_table. If
//// the table is full, an error is returned. Else, the file descriptor table
//// entries are updated with the fd, mode and inode pointer.
//size_t createFileDescriptorEntry(inCoreiNode *inode, size_t mode) {
//    int i;
//    for(i = 0; i < MAX_FD; i++) {
//        if(file_descriptor_table[i].fd == -1) {
//            break;
//        }
//    }
//    if(i == MAX_FD) {
//        // TODO: define errors here
//        return -1;
//    }
//    file_descriptor_table[i].fd = i;
//    file_descriptor_table[i].mode = mode;
//    file_descriptor_table[i].inode = inode;
//    // TODO: CHECK THE MODE, BASED ON THAT, SET THE OFFSET.
//    file_descriptor_table[i].offset = 0;
//    inode -> reference_count ++;
//    return i;
//}
//
//void initFileTableEntries() {
//    int i;
//    for(i = 0; i < MAX_FD; i++) {
//        file_descriptor_table[i].fd = -1;
//    }
//}
