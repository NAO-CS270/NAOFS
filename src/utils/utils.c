#include <stdlib.h>
#include <string.h>
#include "utils/utils.h"

/**
 * Usage:
 * 	char str[] = "hi\nthere";
	char *tokens[2];
	size_t size = strsplit(str, tokens, "\n");
	int i = 0;
	for (; i < size; ++i) {
		printf("%s\n", parts[i]);
	}
 */
int string_split(char* path, char *tokens[], const char *delimiter) {
    int i = 0;
    char *copy = NULL;
    char *token = NULL;
    char *temp = NULL;

    copy = strdup(path);

    if(!copy)
        goto error;

    token = strtok(copy, delimiter);
    temp = strdup(token);
    if(!temp)
        goto error;

    tokens[i++] = temp;

    while(token) {
        token = strtok(NULL, delimiter);
        if(NULL == token)
            break;

        temp = strdup(token);
        if(!temp)
            goto error;
        tokens[i++] = temp;
    }
    free(copy);
    return i;

    error:
        free(copy);
        int k;
        for(k = 0; k < i; k++) {
            free(tokens[k]);
        }
        return -1;
}

// get the parent directory of a file from an absolute path
char* getParentDirectory(const char* path) {
    char* position;
    position = strrchr(path, '/');
    char* parentPath = strndup(path, (path-position+1));
    return parentPath;
}

// extract the filename from an absolute path
char* getFilenameFromPath(const char* path) {
    char *filename = NULL;
    filename = strrchr (path, '/') + 1;
    return filename;
}

