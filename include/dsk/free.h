#ifndef NAOFS_FREE_H
#define NAOFS_FREE_H

#include <stdlib.h>
#include "inode/inCoreiNode.h"

void blockFree(size_t blockNumber);
void inodeBlocksFree(inCoreiNode *inode);

#endif //NAOFS_FREE_H
