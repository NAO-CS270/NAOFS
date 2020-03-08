#include "interface/rename.h"

#include "incoreInodeOps/iget.h"
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
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

int removeDestinationIfExists (inCoreiNode* parentINode, char* filename) {
    directoryEntry *entry = (directoryEntry*)malloc(sizeof(directoryEntry));

    int retVal = searchINodeDirectoryEntries(parentINode, filename, 0, entry, 0, false);
    if (retVal == 1) {
        printf("\nDestination file exists, unlinking it\n");
        inCoreiNode* destination = iget(entry->iNodeNum, 0);

        pthread_mutex_lock(&(destination->iNodeMutex));
        if (destination->type == T_DIRECTORY) {
            retVal = validateAndEmptyDirectoryIfNeeded(destination, true);
            if (retVal < 0) {
                pthread_mutex_unlock(&(destination->iNodeMutex));
                iput(destination);

                free(entry);
                return retVal;
            }
        }
        pthread_mutex_unlock(&(destination->iNodeMutex));
        iput(destination);
        retVal = unlinkFileFromINode(parentINode, entry->name);
        if (retVal < 0) {
            free(entry);
            return retVal;
        }
    }
    return 0;
}

// FUSE does a lot of error checking before even calling this function
// So no need to check a lot of stuff :)
int renameFile(const char* from, const char* to, struct fuse_context* fuseContext) {
    size_t pathLen = strlen(from);
    inCoreiNode *fileINode = getFileINode(from, pathLen);
    
    if (fileINode == NULL) {
        printf("Couldn't find source file\n");
        return -ENOENT;
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

    int retVal = removeDestinationIfExists(toParentINode, newFilename);
    if (retVal < 0) {
        iput(fileINode);

        pthread_mutex_unlock(&(toParentINode->iNodeMutex));
        iput(toParentINode);

        free(newFilename);
        return retVal;
    }    
    //     pthread_mutex_unlock(&(destination->iNodeMutex));
    //     if (destination->type == T_DIRECTORY) {
    //         pthread_mutex_unlock(&(destination->iNodeMutex));
    //         iput(destination);

    //         pthread_mutex_unlock(&(toParentINode->iNodeMutex));
    //         retVal = unlinkDir(to, fuseContext, true);
    //         pthread_mutex_lock(&(toParentINode->iNodeMutex));
    //     }
    //     else if (destination->type == T_REGULAR) {
    //         pthread_mutex_unlock(&(destination->iNodeMutex));
    //         iput(destination);

    //         pthread_mutex_unlock(&(toParentINode->iNodeMutex));
    //         retVal = unlinkFile(to, fuseContext);
    //         pthread_mutex_lock(&(toParentINode->iNodeMutex));            
    //     }

    //     if (retVal != 0) {
    //         printf("Could not rename\n");

    //         pthread_mutex_unlock(&(toParentINode->iNodeMutex));
    //         iput(toParentINode);
    //         iput(fileINode);
    //         free(newFilename);
    //         free(entry);

    //         return -1;
    //     }
    // }

    pthread_mutex_lock(&(fileINode->iNodeMutex));

    getAndUpdateDirectoryTable(toParentINode, fileINode->inode_number, newFilename);
    updateINodeMetadata(fileINode, 0, fileINode->linksCount + 1);

    pthread_mutex_unlock(&(toParentINode->iNodeMutex));
    iput(toParentINode);

    free(newFilename);

    pthread_mutex_unlock(&(fileINode->iNodeMutex));
    iput(fileINode);
    retVal = unlinkFile(from, fuseContext);
    if (retVal != 0) {
        printf("Could not rename\n");
        return -1;
    }

    return 0;
}

