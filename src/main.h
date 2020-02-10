#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include "fileTables.h"
#include "dsk/bmap.h"
#include "dsk/blkfetch.h"
#include "incoreInodeOps/freeList.h"
#include "utils/utils.h"
#include "incoreInodeOps/iput.h"
#include "incoreInodeOps/iget.h"
#include "mkfs/ialloc.h"
#include "trav/directory.h"

#endif //MAIN_H
