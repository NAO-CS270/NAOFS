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

    memcpy(ptrIntoBlock + bmapResp->byteOffsetInBlock, buf, size);

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

    bmapResponse *bmapResp = (bmapResponse *)malloc(sizeof(bmapResponse));
    while (bytesWritten < size) { // TODO: offset doesn't get updated! //BUG
        int retValue = bmap(_fileTableEntry -> inode, offset, bmapResp, APPEND_MODE);
        if (retValue == -1) {
            printf("Reached end of file!!\n");
            break;
        }

        size_t bytesToWrite = min(bmapResp -> bytesLeftInBlock, size - bytesWritten);
        writeToBlock(bmapResp, buf + bytesWritten, bytesToWrite);

        bytesWritten += bytesToWrite;
        _fileTableEntry -> offset += bytesToWrite;
    }

	free(bmapResp);
    return bytesWritten;
}