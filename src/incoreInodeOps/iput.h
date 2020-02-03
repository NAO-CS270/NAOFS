#ifndef IPUT_H
#define IPUT_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "dsk/blkfetch.h"
#include "dsk/mdisk.h"
#include "incoreInodeOps/freeList.h"
#include "incoreInodeOps/hashQ.h"
#include "incoreInodeOps/node.h"
#include "inode/inCoreiNode.h"
#include "mandsk/params.h"

extern void iput(inCoreiNode* inode, Node** hashQ, Node* freelist);
#endif //IPUT_H
