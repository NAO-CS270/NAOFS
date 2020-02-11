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


bool operate(char *workingBuffer, inCoreiNode *workingINode) {
    size_t iNodeNum = findINodeInDirectory(workingINode, workingBuffer);
    if (iNodeNum == 0)
        return false;
    iput(workingINode);
    workingINode = iget(iNodeNum, 0);
    return true;
}


bool processNextLevel(char *path, size_t* counter, char *workingBuffer, inCoreiNode *workingINode) {
    for (int i = 0; ; ++i, ++(*counter)) {
        if ((path[*counter] == '/') || (path[*counter] == '\0')) {
            workingBuffer[i] = 0;
            if (operate(workingBuffer, workingINode) == 0)
                return false;
            break;
        }
        workingBuffer[i] = path[*counter];
    }
    return true;
}


inCoreiNode* getFileINode(char *path, size_t bufLen) {
	size_t pathLen = checkAndGetLen(path, bufLen);
	char *workingBuffer = (char *)malloc((pathLen + 1)*sizeof(char));
	memset(workingBuffer, 0, pathLen + 1);

	inCoreiNode *workingINode = iget(0, 0);

	size_t counter;
	for (counter=0 ;path[counter] != '\0' ; counter++) {
	    if (processNextLevel(path, &counter, workingBuffer, workingINode) == false) {
            return NULL;
	    }
	}
    return workingINode;
}


