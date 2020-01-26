#ifndef _MEMORY_DISK_H
#define _MEMORY_DISK_H

struct disk_block;

extern struct disk_block fetchMemoryDiskBlock(int blockNumber);

#endif
