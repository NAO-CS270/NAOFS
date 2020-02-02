const int SLEEP_TIME_IN_SECONDS = 3;
const int ILIST_START_BLOCK = 3;

inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber, Node** hashQ, Node* freeList) {
    Node* node;
    while(true) {
        node = hash_lookup(deviceNumber, iNodeNumber, hashQ);
        if(NULL != node) {
            inCoreiNode* inode = node->inode;
            if(inode->lock) {
                sleep(SLEEP_TIME_IN_SECONDS);
                continue;
            }
            if(inode->reference_count == 0) {
                // call remove from list.c(freelist)
            }
            inode->reference_count++;
            inode->lock = true;
            return inode;
        }
        if(NULL == freeList) {
            // TODO: Error handling
            return NULL;
        }
        freeList_remove(freeList, node);
        node->inode->inode_number = iNodeNumber;
        node->inode->device_number = deviceNumber;


        insert(node, hashQ);
        // TODO:  call bread here(implemented below for now). Should return void*
        // since the inode numbers start from 0, we don't do iNodeNumber-1.
        int inodeBlockNumber = iNodeNumber / NUM_OF_INODES + ILIST_START_BLOCK;
        disk_block* metaBlock = getDiskBlock(inodeBlockNumber);

        size_t offset = (iNodeNumber % NUM_OF_INODES) * INODE_SIZE;
        iNode* inode = (iNode*)malloc(INODE_SIZE);
        memcpy(inode, metaBlock+offset, INODE_SIZE);
        // end of bread

        node->inode->disk_iNode = inode;
        node->inode->lock = true;
        node->inode->reference_count++;

        // TODO: Update inode status?
        return node->inode;
    }
}