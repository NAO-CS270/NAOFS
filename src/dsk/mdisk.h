#ifndef _MEMORY_DISK_H
#define _MEMORY_DISK_H

struct disk_block {
	unsigned char data[1024];
};
typedef struct disk_block disk_block;

extern disk_block* fetchMemoryDiskBlock(int blockNumber, disk_block* blockData);

extern void writeMemoryDiskBlock(int blockNumber, disk_block* blockData);

#endif
