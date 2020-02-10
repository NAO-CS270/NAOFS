#include "./fileTables.h"

// putFileDescriptorEntry adds an entry to the file_descriptor_table. If
// the table is full, an error is returned. Else, the file descriptor table
// entries are updated with the fd, mode and inode pointer.
size_t putFileDescriptorEntry(inCoreiNode *inode, size_t mode) {
    int i;
    for(i = 0; i < MAX_FD; i++) {
        if(file_descriptor_table[i].fd == -1) {
            break;
        }
    }
    if(i == MAX_FD) {
        // TODO: define errors here
        return -1;
    }
    file_descriptor_table[i].fd = i;
    file_descriptor_table[i].mode = mode;
    file_descriptor_table[i].inode = inode;

    return i;
}

void initFileTableEntries() {
    for(int i = 0; i < MAX_FD; i++) {
        file_descriptor_table[i].fd = -1;
    }
}
