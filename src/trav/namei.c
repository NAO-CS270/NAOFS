#include "trav/directory.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"
#include "utils/utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Ensures that path has only alphanumeric, `_`, `-` or `/` characters. Terminates the path
 * by null byte if it isn't. Returns final path length.
 */
size_t checkAndGetLen(const char *path, size_t bufLen) {
    size_t counter;
    for (counter = 0 ; counter<bufLen ; counter++) {
        debug_print("curr_char: %c buffLen: %d", path[counter], bufLen);
        if (isalnum(path[counter]) != 0) {
            continue;
        }
        if (path[counter] == '_' || path[counter] == '-' || path[counter] == '/') {
            continue;
        }
        if (path[counter] == '\0') {
            return counter + 1;
        }
    }
    return -1;
}

void operate(char *workingBuffer, inCoreiNode *workingINode) {
    size_t iNodeNum = findINodeInDirectory(workingINode, workingBuffer);
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
size_t processNextLevel(const char *path, size_t counter, char *workingBuffer, inCoreiNode *workingINode) {
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
inCoreiNode* getFileINode(const char *path, size_t bufLen) {
    char* truncatedPath = (char *)malloc((bufLen + 1)*sizeof(char));
    memcpy(truncatedPath, path, bufLen);
    truncatedPath[bufLen] = '\0';

	size_t pathLen = checkAndGetLen(truncatedPath, bufLen + 1);
	if (pathLen == -1) {
	    free(truncatedPath);
        return NULL;
    }
    debug_print("pathLen = %d", pathLen);
    char *workingBuffer = (char *)malloc((pathLen)*sizeof(char));
	memset(workingBuffer, 0, pathLen);

	inCoreiNode *workingINode = iget(0, 0);

	size_t counter;
	for (counter=0 ; ; counter++) {
	    debug_print("counter = %d ", counter);
		memset(workingBuffer, 0, pathLen);
		counter = processNextLevel(truncatedPath, counter, workingBuffer, workingINode);

		if (workingINode == NULL) {
			return NULL;
		}

		if (truncatedPath[counter] == '\0') {
			break;
		}
	}
	free(truncatedPath);
    return workingINode;
}

