#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#define FUSE_USE_VERSION 29
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include "./fileTables.h"
#include "./mkfs/bmap.h"
#include "./incoreInodeOps/freeList.h"

#endif //MAIN_H
