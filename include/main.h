#ifndef MAIN_H
#define MAIN_H

#include "fileTables.h"
#include "mkfs/bmap.h"
#include "incoreInodeOps/freeList.h"
#include "fileTables.h"
#include "incoreInodeOps/bmap.h"
#include "dsk/blkfetch.h"
#include "utils/utils.h"
#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/iget.h"
#include "mkfs/ialloc.h"
#include "trav/directory.h"
#define FUSE_USE_VERSION 29
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>


#endif //MAIN_H
