#include "utils/utils.h"

#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

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

/*
#define UNW_LOCAL_ONLY
#include <libunwind.h>

void print_backtrace (void) {
  unw_cursor_t cursor; unw_context_t uc;
  unw_word_t ip, sp;

  unw_getcontext(&uc);
  unw_init_local(&cursor, &uc);
  while (unw_step(&cursor) > 0) {
    unw_get_reg(&cursor, UNW_REG_IP, &ip);
    unw_get_reg(&cursor, UNW_REG_SP, &sp);
    printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
  }
}
*/
