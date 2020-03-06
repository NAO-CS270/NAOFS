#ifndef _DISK_ACCESS_H
#define _DISK_ACCESS_H

#include "dsk/mdisk.h"

#include <stddef.h>

extern int initDeviceAccessor(const char *inputDev);
extern int fetchDeviceDiskBlock(size_t blockNumber, disk_block *blockData);
extern int writeDeviceDiskBlock(size_t blockNumber, disk_block* blockData);

#endif
