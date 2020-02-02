#include <stdbool.h>

#include "dsk/blkfetch.h"
#include "inCoreInodeOps/iput.h"
#include "mandsk/params.h"


void iput(inCoreiNode* inode, Node** hashQ, Node* freelist) {
    if(inode->lock == false) {
        inode->lock = true;
        inode->reference_count--;
    }
    if(inode->reference_count == 0) {
        /** TODO: include link_count in the inCoreiNode struct
         if(inode->link_count == 0) {
             // handle file deletion here
 //            free disk blocks for file (algorithm: free); // free is described later
 //			set file type to 0;
 //			free inode (algorithm: ifree); // ifree is described later
         } */

        // TODO: take care of when file_data_changed is TRUE
        // TODO: Take a global lock
        if (inode->inode_changed && !inode->file_data_changed) {
            int inodeBlockNumber = inode->inode_number / NUM_OF_INODES + ILIST_START_BLOCK;
            disk_block *metaBlock = getDiskBlock(inodeBlockNumber);

            size_t offset = (inode->inode_number % NUM_OF_INODES) * INODE_SIZE;
            memcpy(metaBlock + offset, inode->disk_iNode, INODE_SIZE);
            writeDiskBlock(inodeBlockNumber, metaBlock);
        }
        Node* node = hashLookup(inode->device_number, inode->inode_number, hashQ);
        if(NULL == node) {
            // TODO: handle this exception
        }
        freeListInsert(freelist, node);
    }
    inode->lock = false;
}