#include "interface/unlink.h"

#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/iNodeManager.h"
#include "inode/inCoreiNode.h"
#include "interface/create.h"
#include "interface/read.h"
#include "interface/truncate.h"
#include "mandsk/params.h"
#include "trav/namei.h"
#include "trav/directory.h"
#include "utils/utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

int unlinkFileFromINode(inCoreiNode* parentINode, char* filename) {
    directoryEntry *entry = (directoryEntry*)malloc(sizeof(directoryEntry));
    int retVal = searchINodeDirectoryEntries(parentINode, filename, 0, entry, 0, true);

    if (retVal != 1) {
        printf("Could not remove the file");
        free(entry);
        return -1;
    }

    inCoreiNode *deletedFile = iget(entry->iNodeNum, 0);
    pthread_mutex_lock(&(deletedFile->iNodeMutex));

    printf ("In unlink, linksCount: %ld\n", deletedFile->linksCount);
    deletedFile->linksCount -= 1;
    updateINodeMetadata(deletedFile, 0, deletedFile->linksCount);
    printf ("In unlink, linksCount: %ld\n", deletedFile->linksCount);
    
    pthread_mutex_unlock(&(deletedFile->iNodeMutex));
    iput(deletedFile);

    truncateINode (parentINode, parentINode->size - DIRECTORY_ENTRY_SIZE);
    free(entry);

    return 0;
}

int unlinkFile(const char* path, struct fuse_context* fuseContext) {
    size_t pathLen = strlen(path);
    char *filename = (char *)malloc((pathLen + 1)*sizeof(char));

    inCoreiNode *parentINode = getParentINode(path, pathLen);
    if (parentINode == NULL) {
        printf("Couldn't find directory to create iNode\n");
        return -ENOENT;
    }

    pthread_mutex_lock(&(parentINode->iNodeMutex));

    getFilenameFromPath(path, filename);

    int retVal = unlinkFileFromINode(parentINode, filename);
    if (retVal == -1) {
        pthread_mutex_unlock(&(parentINode->iNodeMutex));
        iput(parentINode);
        free(filename);
        return -1;
    }

    pthread_mutex_unlock(&(parentINode->iNodeMutex));
    iput(parentINode);

    free(filename);
    return 0;
    // inCoreiNode* parentInode = validateThenGetParentINode(T_REGULAR, file, filename);

    /**
     * search for the directory entry in teh parent dir
     * save the pointer of that entry. So that we dont have to read from the disk again
     * bmap - get the last directory entry
     * memcpy into the new location
     * write the mempy-ed disk block into the disk
     * iget on the unlinked inode number.
    // the book says that if unlinking the current directory... */
    // if (last component of file name is ".")
    // increment mode reference count;

    // TODO: if file is DIR but user is not super user, return error(refer book)



    //write parent directory: zero inode number of unlinked file; release inode parent directory (algorithm iput);
}

int emptyDir(inCoreiNode *fileINode) {
    size_t entriesInDirectory = (fileINode->size) / DIRECTORY_ENTRY_SIZE;
    size_t entriesOffset = 2 * DIRECTORY_ENTRY_SIZE;

    directoryEntry *entry = (directoryEntry *)malloc(sizeof(directoryEntry));
    while (entriesOffset < entriesInDirectory) {
        memset(entry, 0, sizeof(directoryEntry));
        getLastEntry(fileINode, entry);
        inCoreiNode* fileEntry = iget(entry->iNodeNum, 0);
        pthread_mutex_lock(&(fileEntry->iNodeMutex));
        if (fileEntry->type == T_DIRECTORY) {
            int retVal = validateAndEmptyDirectoryIfNeeded(fileEntry, true);
            if (retVal < 0) {
                pthread_mutex_unlock(&(fileEntry->iNodeMutex));
                iput(fileEntry);
                free(entry);
                return retVal;
            }
        }
        pthread_mutex_unlock(&(fileEntry->iNodeMutex));
        iput(fileEntry);
        unlinkFileFromINode(fileINode, entry->name);
        entriesOffset += DIRECTORY_ENTRY_SIZE;
    }
    free(entry);

    return 0;
}
//     while (entriesOffset < entriesInDirectory) {
//         memset(entryBuffer, 0, entriesPerIteration * sizeof(directoryEntry));
//         entriesRead = searchINodeDirectoryEntries(fileINode, NULL, entriesOffset, entryBuffer, entriesPerIteration, false);

//         counter = 0;

//         while (counter < entriesRead) {
//             char* filename = (entryBuffer + counter)->name;
//             size_t filenameLen = strlen(filename);
//             char* newPath = (char*)malloc(pathLen+filenameLen+1);
//             memcpy(newPath, path, pathLen);
//             memcpy(newPath + pathLen, "/", 1);
//             memcpy(newPath + pathLen + 1, filename, filenameLen);
//             printf("\n--------------------------\nYOYOYOYOYOYOYOYOYOYO\n\n");
//             printf("%s\n", newPath);

//             pthread_mutex_unlock(&(fileINode->iNodeMutex));
//             inCoreiNode* destination = iget((entryBuffer + counter)->iNodeNum, 0);
//             pthread_mutex_lock(&(destination->iNodeMutex));
//             if (destination->type == T_DIRECTORY) {
//                 pthread_mutex_unlock(&(destination->iNodeMutex));
//                 iput(destination);

//                 unlinkDir(newPath, fuseContext, true);
//             }
//             else if (destination->type == T_REGULAR) {
//                 pthread_mutex_unlock(&(destination->iNodeMutex));
//                 iput(destination);

//                 unlinkFile(newPath, fuseContext);
//             }

//             pthread_mutex_lock(&(fileINode->iNodeMutex));
//             free(newPath);

//             counter++;
//         }

//         entriesOffset += entriesRead;
//     }
//     free(entryBuffer);
//     pthread_mutex_unlock(&(fileINode->iNodeMutex));
// }

int validateAndEmptyDirectoryIfNeeded (inCoreiNode *fileINode, bool force) {
    if (fileINode->inode_number == 0) {
        printf("Cannot delete root directory\n");
        return -EACCES;
    }
    if (fileINode->size > (2 * DIRECTORY_ENTRY_SIZE) && !force) {
        printf("Directory not empty\n");
        return -ENOTEMPTY;
    }
    if (fileINode->type != T_DIRECTORY) {
        printf("Given path is not a directory\n");
        return -ENOTDIR;
    }
    if (force && (fileINode->size > (2 * DIRECTORY_ENTRY_SIZE))) {
       return emptyDir(fileINode);
    }
    return 0;
}

int unlinkDir (const char* path, struct fuse_context* fuseContext, bool force) {
    size_t pathLen = strlen(path);
    inCoreiNode *fileINode = getFileINode(path, pathLen);
    pthread_mutex_lock(&(fileINode->iNodeMutex));

    int retVal = validateAndEmptyDirectoryIfNeeded (fileINode, force);
    if (retVal < 0) {
        pthread_mutex_unlock(&(fileINode->iNodeMutex));
        iput(fileINode);

        return retVal;
    }

    pthread_mutex_unlock(&(fileINode->iNodeMutex));
    iput(fileINode);

    return unlinkFile(path, fuseContext);
}
