#ifndef NAOFS_FREE_H
#define NAOFS_FREE_H

#include "inode/inCoreiNode.h"

#include <stddef.h>

void blockFree(size_t blockNumber);
void inodeBlocksFree(inCoreiNode *inode);

#endif

