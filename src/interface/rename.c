#include "interface/rename.h"

#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/iNodeManager.h"
#include "inode/inCoreiNode.h"
#include "interface/create.h"
#include "interface/unlink.h"
#include "trav/namei.h"
#include "trav/directory.h"
#include "utils/utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

// FUSE does a lot of error checking before even calling this function
// So no need to check a lot of stuff :)
int renameFile(const char* from, const char* to, struct fuse_context* fuseContext) {
    size_t pathLen = strlen(from);
    inCoreiNode *fileINode = getFileINode(from, pathLen);
    
    if (fileINode == NULL) {
        printf("Couldn't find source file\n");
        return -ENOENT;
    }

    // TODO: Remove this once unlink directory is implemented
    if (fileINode->type == T_DIRECTORY) {
        printf("MOVING DIRECTORY NOT YET IMPLEMENTED!!\n");
        iput(fileINode);
        return -1;
    }

    pathLen = strlen(to);
    char *newFilename = (char *)malloc((pathLen + 1)*sizeof(char));

    inCoreiNode *toParentINode = getParentINode(to, pathLen);
    if (toParentINode == NULL) {
        printf("Couldn't find destination parent directory\n");
        return -ENOENT;
    }
    pthread_mutex_lock(&(toParentINode->iNodeMutex));

    getFilenameFromPath(to, newFilename);

    directoryEntry *entry = (directoryEntry*)malloc(sizeof(directoryEntry));

    int retVal = searchINodeDirectoryEntries(toParentINode, newFilename, 0, entry, 0, false);
    if (retVal == 1) {
        printf("Destination file exists, unlinking it\n");
        retVal = unlinkFile(to, fuseContext);

        if (retVal != 0) {
            printf("Could not rename\n");

            pthread_mutex_unlock(&(toParentINode->iNodeMutex));
            iput(toParentINode);
            iput(fileINode);
            free(newFilename);
            free(entry);

            return -1;
        }
    }

    pthread_mutex_lock(&(fileINode->iNodeMutex));

    getAndUpdateDirectoryTable(toParentINode, fileINode->inode_number, newFilename);
    updateINodeMetadata(fileINode, 0, fileINode->linksCount + 1);

    pthread_mutex_unlock(&(toParentINode->iNodeMutex));
    iput(toParentINode);

    free(newFilename);
    free(entry);

    // TODO: Add unlink dir if file type is directory
    if (fileINode->type == T_REGULAR) {
        pthread_mutex_unlock(&(fileINode->iNodeMutex));
        iput(fileINode);
        retVal = unlinkFile(from, fuseContext);
    }
    if (retVal != 0) {
        printf("Could not rename\n");
        return -1;
    }

    return 0;
}

