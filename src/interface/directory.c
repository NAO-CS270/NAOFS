#include "interface/directory.h"
#include "utils/utils.h"
#include "inode/inCoreiNode.h"
#include "trav/namei.h"
#include "inode/iNode.h"
#include "trav/directory.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"
#include "mkfs/ialloc.h"

#include <time.h>
#include <string.h>

/* 
 * Edge cases:
 * Handle the directory to create is `..`.
 * No linking was done. Is it needed?
 * Need to add file descriptor entry?
 */
int createDirectory(const char* path, mode_t mode) {
	size_t pathLen = strlen(path);
	char *parentDirPath = (char *)malloc((pathLen + 1)*sizeof(char));
	char *filename = (char *)malloc((pathLen + 1)*sizeof(char));

	getParentDirectory(path, parentDirPath);
	inCoreiNode *parentINode = getFileINode(parentDirPath, strlen(parentDirPath));
	if (parentINode == NULL) {
		printf("Couldn't find directory to create iNode\n");
		return -1;
	}

	directoryEntry *entryBuffer = (directoryEntry *)malloc(sizeof(directoryEntry));
    getFilenameFromPath(path, filename);
	int retValue = searchINodeDirectoryEntries(parentINode, filename, 0, entryBuffer, 0);
	size_t iNodeNum = entryBuffer->iNodeNum;
	if (retValue == 1) {
		printf("A file with this name is found in the directory!!\n");
		return -1;
	}

	iNodeNum = getNewINode(T_DIRECTORY, P_RUSR | P_WUSR | P_XUSR | P_RGRP | P_ROTH);
	printf("Creating directory %s in %s with iNode number: %ld\n", filename, parentDirPath, iNodeNum);
    getAndUpdateDirectoryTable(parentINode, iNodeNum, filename);

    inCoreiNode *newINode = iget(iNodeNum, 0);
    newINode -> type = T_DIRECTORY;
    updateNewDirMetaData(newINode, parentINode->inode_number);

	iput(parentINode);
    iput(newINode);

	free(parentDirPath);
	free(filename);
	free(entryBuffer);
	return 0;
}

void fillDirectoryEntries(fuse_fill_dir_t filler, void *buffer, inCoreiNode *iNodePtr) {
	size_t entriesInDirectory = (iNodePtr->size) / DIRECTORY_ENTRY_SIZE;
	size_t entriesPerIteration = BLOCK_SIZE / DIRECTORY_ENTRY_SIZE;
	size_t entriesOffset = 0;
	size_t entriesRead;
	size_t counter;

	directoryEntry *entryBuffer = (directoryEntry *)malloc(entriesPerIteration * sizeof(directoryEntry));

	while (entriesOffset < entriesInDirectory) {
		memset(entryBuffer, 0, entriesPerIteration * sizeof(directoryEntry));
		entriesRead = searchINodeDirectoryEntries(iNodePtr, NULL, entriesOffset, entryBuffer, entriesPerIteration);

		counter = 0;
		while (counter < entriesRead) {
			filler(buffer, (entryBuffer + counter)->name, NULL, 0);
			counter++;
		}
		entriesOffset += entriesRead;
	}
	free(entryBuffer);
}

int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    inCoreiNode* iNodeOfPath = getFileINode(path, strlen(path));
    if(iNodeOfPath == NULL) {
        debug_print("iNode not found for path: %s\n", path);
        return -1;
    }

	fillDirectoryEntries(filler, buf, iNodeOfPath);

	iput(iNodeOfPath);
    return 0;
}

