#ifndef NAOFS_FREE_H
#define NAOFS_FREE_H

#include <stdlib.h>
#include "inode/inCoreiNode.h"

extern void blockFree(size_t blockNumber);
extern void inodeBlocksFree(inCoreiNode *inode);

#endif //NAOFS_FREE_H
