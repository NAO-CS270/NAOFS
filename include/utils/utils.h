#ifndef NAOFS_UTILS_H
#define NAOFS_UTILS_H


#define debug_print(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)


extern char* getParentDirectory(const char* path);
extern char* getFilenameFromPath(const char* path);

#endif