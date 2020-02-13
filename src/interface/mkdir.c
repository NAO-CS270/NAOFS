#include "interface/mkdir.h"
#include "utils/utils.h"
#include "inode/inCoreiNode.h"
#include "trav/namei.h"
#include "inode/iNode.h"
#include "trav/directory.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"
#include "mkfs/ialloc.h"

#include <string.h>

/* 
 * Edge cases:
 * Handle the directory to create is `..`.
 * No linking was done. Is it needed?
 * Need to add file descriptor entry?
 */
int createDirectory(const char* path, mode_t mode) {
    char *parentDirPath = getParentDirectory(path);
	inCoreiNode *parentINode = getFileINode(parentDirPath, strlen(parentDirPath));
	if (parentINode == NULL) {
		printf("Couldn't find directory to create iNode\n");
		return -1;
	}

    char *filename = getFilenameFromPath(path);
	size_t iNodeNum = findINodeInDirectory(parentINode, filename);
	if (iNodeNum != 0) {
		printf("A file with this name is found in the directory!!\n");
		return -1;
	}

	iNodeNum = getNewINode();
	printf("Creating directory %s in %s with iNode number: %ld\n", filename, parentDirPath, iNodeNum);
    getAndUpdateDirectoryTable(parentINode, iNodeNum, filename);

    inCoreiNode *newINode = iget(iNodeNum, 0);
    newINode -> type = T_DIRECTORY;
    updateNewDirMetaData(newINode, parentINode->inode_number);

	iput(parentINode);
    iput(newINode);
	return 0;
}

