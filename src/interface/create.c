#include "utils/utils.h"
#include "trav/namei.h"
#include "trav/directory.h"
#include "inode/iNode.h"
#include "inode/inCoreiNode.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"
#include "interface/open.h"
#include "mkfs/ialloc.h"
#include "incoreInodeOps/iNodeManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


// TODO: find a location for this function, same function copied in unlink
inCoreiNode *getParentINode(const char *path, size_t pathLen) {
	char *parentDirPath = (char *)malloc((pathLen + 1)*sizeof(char));

	getParentDirectory(path, parentDirPath);
	inCoreiNode *parentINode = getFileINode(parentDirPath, strlen(parentDirPath));

	free(parentDirPath);
	return parentINode;
}

bool doesFileExistIn(char *filename, inCoreiNode *parentINode) {
	directoryEntry *entryBuffer = (directoryEntry *)malloc(sizeof(directoryEntry));
	int retValue = searchINodeDirectoryEntries(parentINode, filename, 0, entryBuffer, 0, false);
	free(entryBuffer);

	if (retValue > 0) {
		return true;
	}
	return false;
}

inCoreiNode *validateThenGetParentINode(iNodeType fileType, const char *path, char *filename) {
	if (fileType != T_DIRECTORY && fileType != T_REGULAR) {
		printf("Unsupported file type\n");
		return NULL;
	}

	size_t pathLen = strlen(path);
	inCoreiNode *parentINode = getParentINode(path, pathLen);
	if (parentINode == NULL) {
		printf("Couldn't find directory to create iNode\n");
		return NULL;
	}

    getFilenameFromPath(path, filename);
	if (doesFileExistIn(filename, parentINode)) {
		printf("A file with this name is found in the directory!!\n");
		return NULL;
	}

	return parentINode;
}

int createFile(const char *path, iNodeType fileType, mode_t mode, struct fuse_context *fuse_context) {
	printf("Here it is, Here is everything - %d\n", mode);
	size_t pathLen = strlen(path);
	char *filename = (char *)malloc((pathLen + 1)*sizeof(char));
	
	inCoreiNode *parentINode = validateThenGetParentINode(fileType, path, filename);
	if (parentINode == NULL) {
		return -1;
	}

	if (fileType == T_DIRECTORY) {
		mode = S_IFDIR | mode;
	}
	printf("fuse context UID: %d, GID: %d\n\n", fuse_context->uid, fuse_context->gid);
	size_t iNodeNum = getNewINode(fileType, mode, fuse_context->uid, fuse_context->gid);
    getAndUpdateDirectoryTable(parentINode, iNodeNum, filename);

    inCoreiNode *newINode = iget(iNodeNum, 0);
    if (fileType == T_REGULAR) {
        updateINodeMetadata(newINode, 0, 1);
    }

    if (fileType == T_DIRECTORY) {
		updateNewDirMetaData(newINode, parentINode->inode_number);
    }

	iput(parentINode);
    iput(newINode);

	free(filename);
	return 0;
}

