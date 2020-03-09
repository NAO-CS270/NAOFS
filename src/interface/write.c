#include "dsk/node.h"
#include "incoreInodeOps/bmap.h"
#include "fdTable/globalFileTable.h"
#include "incoreInodeOps/iNodeManager.h"
#include "mandsk/params.h"
#include "dsk/blkfetch.h"
#include "utils/utils.h"
#include <string.h>
#include <fuse.h>

void writeToBlock(bmapResponse *bmapResp, const char *buf, size_t size) {
	cacheNode *dataBlockNode = getDiskBlockNode(bmapResp->blockNumber, 0);
    unsigned char *ptrIntoBlock = dataBlockNode->dataBlock->data;

    memcpy(ptrIntoBlock + bmapResp->byteOffsetInBlock, buf, size);
	dataBlockNode->header->delayedWrite = true;
    
    writeDiskBlockNode(dataBlockNode);
}

int writeToFile(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi, struct fuse_context *fuse_context) {
    printf("\nBEGINNING OF WRITE_TO_FILE\n");
    printf("Size: %ld, offset: %ld\n", size, offset);
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
    if (offset > _fileTableEntry->inode->size) {
        printf ("Offset past end of file.\n");
        return -1;
    }
    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
    while (bytesWritten < size) {

        int retValue = bmap(_fileTableEntry -> inode, offset, bmapResp, APPEND_MODE);
        if (retValue < 0) {
            printf("[writeToFile] Error returned from bmap: %d!\n", retValue);
            break;
        }
        size_t bytesToWrite = min(bmapResp -> bytesLeftInBlock, size - bytesWritten);
        writeToBlock(bmapResp, buf + bytesWritten, bytesToWrite);

        if (offset == _fileTableEntry->inode->size) {
            updateINodeMetadata(_fileTableEntry->inode, bytesToWrite, _fileTableEntry->inode->linksCount);
        }
        else if ((_fileTableEntry->inode->size - bytesWritten) < (size - bytesWritten)) {
            updateINodeMetadata(_fileTableEntry->inode, bytesToWrite - _fileTableEntry->inode->size, _fileTableEntry->inode->linksCount);
        }

        bytesWritten += bytesToWrite;
        _fileTableEntry -> offset += bytesToWrite; //TODO: Why is it necessary?
        offset += bytesToWrite;

        printf("size of the file: %ld\n", _fileTableEntry->inode->size);
        printf("bytes written: %d\n", bytesWritten);
        printf("offset: %ld\n", offset);
    }
    pthread_mutex_unlock(&(_fileTableEntry->inode->iNodeMutex));
    free(bmapResp);
    return bytesWritten;
}
