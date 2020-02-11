#include "trav/directory.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Ensures that path has only alphanumeric, `_`, `-` or `/` characters. Terminates the path
 * by null byte if it isn't. Returns final path length.
 */
size_t checkAndGetLen(char *path, size_t bufLen) {
    size_t counter;
    for (counter = 0 ; counter<bufLen ; counter++) {
        if (isalnum(path[counter]) != 0) {
            continue;
        }
        if (path[counter] == '_' || path[counter] == '-' || path[counter] == '/') {
            continue;
        }
        if (path[counter] == '\0') {
            return counter;
        }
    }
    path[bufLen-1] = '\0';
    return bufLen;
}

void operate(char *workingBuffer, inCoreiNode *workingINode) {
    size_t iNodeNum = findINodeInDirectory(workingINode, workingBuffer);
    if (iNodeNum == 0)
        return false;
    iput(workingINode);
	if (iNodeNum == 0) {
		workingINode = NULL;
	}
	else {
		workingINode = iget(iNodeNum, 0);
	}
}

/**
 * Assumes the string `path`, starting at `counter` is the name of a file in the directory of `workingINode`. Then
 * tries to find the file in the directory.
 */
size_t processNextLevel(char *path, size_t counter, char *workingBuffer, inCoreiNode *workingINode) {
    for (; ; counter++) {
        if ((path[counter] == '/') || (path[counter] == '\0')) {
            operate(workingBuffer, workingINode);
            break;
        }
        workingBuffer[counter] = path[counter];
    }
    return counter;
}

/* Takes in path string in `path` and the length of it in `bufLen`. Returns `NULL` if any of the directories in
 * along the path doesn't exist.
 */
inCoreiNode* getFileINode(char *path, size_t bufLen) {
	size_t pathLen = checkAndGetLen(path, bufLen);
	char *workingBuffer = (char *)malloc((pathLen + 1)*sizeof(char));
	memset(workingBuffer, 0, pathLen + 1);

	inCoreiNode *workingINode = iget(0, 0);

	size_t counter;
	for (counter=0 ; ; counter++) {
		memset(workingBuffer, 0, pathLen + 1);
		counter = processNextLevel(path, counter, workingBuffer, workingINode);

		if (workingINode == NULL) {
			return NULL;
		}

		if (path[counter] == '\0') {
			break;
		}
	}
    return workingINode;
}

