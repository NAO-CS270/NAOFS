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
size_t checkAndGetLen(const char *path, size_t bufLen, char *pathBuf) {
    size_t counter;
    for (counter = 0 ; counter < bufLen ; counter++) {
		pathBuf[counter] = path[counter];
        if (isalnum(path[counter]) != 0) {
            continue;
        }
        if (path[counter] == '_' || path[counter] == '-' || path[counter] == '/') {
            continue;
        }
        if (path[counter] == '\0') {
			break;
        }
    }
	pathBuf[counter] = '\0';
	return counter;
}

int operate(const char *path, size_t startAt, size_t endAt, inCoreiNode *workingINode) {
	if (startAt == endAt) {
		return -1;
	}
	size_t bufLen = endAt - startAt;
	char *workingBuffer = (char *)malloc(sizeof(char) * (bufLen + 1));
	memcpy(workingBuffer, path+startAt, bufLen);
	workingBuffer[bufLen] = '\0';

    size_t iNodeNum = findINodeInDirectory(workingINode, workingBuffer);
	free(workingBuffer);

    iput(workingINode);
	if (iNodeNum == 0) {
		return -1;
	}
	workingINode = iget(iNodeNum, 0);
	return 0;
}

/**
 * Assumes the string `path`, starting at `counter` is the name of a file in the directory of `workingINode`. Then
 * tries to find the file in the directory.
 */
size_t processNextLevel(const char *path, size_t startAt, inCoreiNode *workingINode) {
	size_t endAt = startAt;
    while(1) {
        if ((path[endAt] == '/') || (path[endAt] == '\0')) {
            break;
        }
		endAt++;
    }
    size_t retValue = operate(path, startAt, endAt, workingINode);
	if (retValue == -1) {
		return -1;
	}
	if (path[endAt] == '\0') {
		return endAt - 1;
	}
    return endAt;
}

/* Takes in path string in `path` and the length of it in `bufLen`. Returns `NULL` if any of the directories in
 * along the path doesn't exist.
 */
inCoreiNode* getFileINode(const char *path, size_t bufLen) {
    char *pathBuffer = (char *)malloc((bufLen + 1)*sizeof(char));
	size_t pathLen = checkAndGetLen(path, bufLen, pathBuffer);
    
	debug_print("Given path - %s, length = %ld\n", path, pathLen);
	inCoreiNode *workingINode = iget(0, 0);

	size_t counter;
	for (counter=1 ; ; counter++) {
		if (path[counter] == '\0') {
			break;
		}
		counter = processNextLevel(path, counter, workingINode);

		if (counter == -1) {
			workingINode = NULL;
			break;
		}
	}
	free(pathBuffer);
    return workingINode;
}

