#ifndef NAOFS_NAMEI_H
#define NAOFS_NAMEI_H

#include "inode/inCoreiNode.h"

inCoreiNode* getFileINode(const char *path, size_t bufLen);

#endif //NAOFS_NAMEI_H
