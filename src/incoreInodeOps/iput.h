#ifndef IPUT_H
#define IPUT_H

#include "incoreInodeOps/freelist.h"
#include "incoreInodeOps/hashQ.h"
#include "inode/inCoreiNode.h"

extern void iput(inCoreiNode* inode, Node** hashQ, Node* freelist);
#endif //IPUT_H
