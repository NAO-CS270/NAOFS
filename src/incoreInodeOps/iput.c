#include "incoreInodeOps/iput.h"

void iput(inCoreiNode* inode) {
    // take lock, the thread calling iput should release lock and call it
    pthread_mutex_lock(&(inode->iNodeMutex));
    inode->reference_count--;
    if(inode->reference_count == 0) {
        /** TODO: include link_count in the inCoreiNode struct
         if(inode->link_count == 0) {
             // handle file deletion here
 //            free disk blocks for file (algorithm: free); // free is described later
 //			set file type to 0;
 //			free inode (algorithm: ifree); // ifree is described later
         } */

        // writes inode data to disk
        if (inode->inode_changed && !inode->file_data_changed) {
            writeDiskInode(inode);
        }
        // get hash Q node
        Node* node = hashLookup(inode->device_number, inode->inode_number);
        if(NULL == node) {
            // TODO: handle this exception
        }
        // remove it from free inode list
        freeListInsert(node);
    }
    pthread_mutex_unlock(&(inode->iNodeMutex));
}
