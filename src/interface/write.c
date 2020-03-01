#include "incoreInodeOps/bmap.h"
#include "fdTable/fileTables.h"
#include "dsk/blkfetch.h"
#include "utils/utils.h"
#include <string.h>
#include <fuse.h>

void writeToBlock(bmapResponse *bmapResp, const char *buf, size_t size) {
    disk_block *blockPtr = (disk_block *)malloc(sizeof(disk_block));
    getDiskBlock(bmapResp->blockNumber, blockPtr);
    unsigned char *ptrIntoBlock = blockPtr->data;

        printf("WE ARE IN WRITE!\n");
    printf("block_number: %d\n", bmapResp->blockNumber);
    int i = 0;
    for(i=0; i < size; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
    memcpy(ptrIntoBlock + bmapResp->byteOffsetInBlock, buf, size);
    
    for(i=0; i < size; i++) {
        printf("%c", (ptrIntoBlock + bmapResp->byteOffsetInBlock)[i]);
    }
    printf("\n");
    writeDiskBlock(bmapResp->blockNumber, blockPtr);
    free(blockPtr);
}

int writeToFile(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context) {
    printf("filehandler: %ld", fi->fh);
    if(fi->fh < 0) { // TODO: Do we really need this check?
        return -1;
    }
    int bytesWritten = 0;
    int error = 0;

    fileTableEntry* _fileTableEntry = getFileDescriptor(fuse_context -> pid, fi -> fh, &error);
    if(_fileTableEntry == NULL) {
        printf("_fileTableEntry is NULL\n");
        //break;
        return -1;
    }
    pthread_mutex_lock(&(_fileTableEntry->inode->iNodeMutex));
    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
    while (bytesWritten < size) { // TODO: offset doesn't get updated! //BUG
        int retValue = bmap(_fileTableEntry -> inode, offset, bmapResp, APPEND_MODE);

        size_t bytesToWrite = min(bmapResp -> bytesLeftInBlock, size - bytesWritten);
        writeToBlock(bmapResp, buf + bytesWritten, bytesToWrite);

        bytesWritten += bytesToWrite;
        _fileTableEntry -> offset += bytesToWrite;
    }
    _fileTableEntry->inode->size += bytesWritten;
    pthread_mutex_unlock(&(_fileTableEntry->inode->iNodeMutex));
    free(bmapResp);
    return bytesWritten;
}
