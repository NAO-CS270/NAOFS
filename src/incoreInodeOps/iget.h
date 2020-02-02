#ifndef IGET_H
#define IGET_H

#include <stdbool.h>
#include <unistd.h>

#include "disk/mdisk.h"
#include "incoreInodeOps/hashQ.h"
#include "incoreInodeOps/freeList.h"
#include "incoreInodeOps/node.h"
#include "inode/inCoreiNode.h"
#include "inode/iNode.h"
#include "mandsk/params.h"

extern inCoreiNode* iget(size_t iNodeNumber, size_t deviceNumber, Node** hashQ, Node* freeList);

#endif //IGET_H
