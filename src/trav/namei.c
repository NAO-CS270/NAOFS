#include "trav/directory.h"
#include "incoreInodeOps/iget.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void getFileINode(char *path, size_t bufLen, inCoreiNode *iNodeData) {
	size_t pathLen = checkAndGetLen(path, bufLen);
	char *workingBuffer = (char *)malloc((pathLen + 1)*sizeof(char));
	memset(workingBuffer, 0, pathLen + 1);

	inCoreiNode *workingINode = iget(0, 0);

	size_t counter;
	for (counter=0 ; ; counter++) {
		memset(workingBuffer, 0, pathLen + 1);
		counter += processNextLevel(path, workingBuffer, workingINode);

		if (path[counter] == '\0') {
			break;
		}
	}
}

size_t processNextLevel(char *path, char *workingBuffer, inCoreiNode *workingINode) {
	size_t counter;
	for (counter=0 ; ; counter++) {
		if ((path[counter] == '/') || (path[counter] == '\0')) {
			operate(workingBuffer, workingINode);
			break;
		}
		workingBuffer[counter] = path[counter];
		counter++;
	}
	return counter;
}

void operate(char *workingBuffer, inCoreiNode *workingINode) {
	size_t iNodeNum = findINodeInDirectory(workingINode, workingBuffer);
	iput(workingINode);
	workingINode = iget(iNodeNum, 0);
}

/* Ensures that path has only alphanumeric, `_`, `-` or `/` characters. Terminates the path
 * by null byte if it isn't. Returns final path length.
 */
void checkAndGetLen(char *path, size_t bufLen) {
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

