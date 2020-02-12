#ifndef NAOFS_UTILS_H
#define NAOFS_UTILS_H


#define debug_print(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): \n" fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

extern int string_split(char*, char *[], const char*);
extern char* getParentDirectory(const char* path);
extern char* getFilenameFromPath(const char* path);

#endif