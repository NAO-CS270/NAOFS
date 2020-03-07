#include "interface/unlink.h"

#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/iNodeManager.h"
#include "inode/inCoreiNode.h"
#include "interface/create.h"
#include "interface/truncate.h"
#include "mandsk/params.h"
#include "trav/namei.h"
#include "trav/directory.h"
#include "utils/utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

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

    directoryEntry *entry = (directoryEntry*)malloc(sizeof(directoryEntry));
    int retVal = searchINodeDirectoryEntries(parentINode, filename, 0, entry, 0, true);
    if (retVal != 1) {
        printf("Could not remove the file");

        pthread_mutex_unlock(&(parentINode->iNodeMutex));
        iput(parentINode);
        free(filename);
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

    pthread_mutex_unlock(&(parentINode->iNodeMutex));
    iput(parentINode);

    free(filename);
    free(entry);
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
    //iput(inode);
    // iput(parentInode);
}