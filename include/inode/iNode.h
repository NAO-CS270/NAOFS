#ifndef INODE_H
#define INODE_H

#include "dsk/mdisk.h"
#include "mandsk/params.h"

#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define BLOCKS_IN_INODE 13

enum iNodeType {
	T_FREE		= 0,
	T_REGULAR	= 1,
	T_DIRECTORY	= 2
};
typedef enum iNodeType iNodeType;

// Using this struct in the project. Keeping the old for for reference.
struct iNode {
    // device number
	size_t device_number;

    // inode number
	size_t inode_number; 

    // times accessed, modified and status changed
	time_t access;
	time_t modification;
	time_t status_change;

    // link count
	size_t linksCount;

    // type of the file
	iNodeType type;

    // file permissions
	mode_t file_mode;

    // id of the owner
	uid_t owner_uid;

    // if of the group
	gid_t group_uid;

	size_t dataBlockNums[BLOCKS_IN_INODE]; // same as the data block defined above, must remove one of them

	size_t size;
};
typedef struct iNode iNode;

extern void initializeINode(iNode *iNodePtr, size_t iNodeNum);
extern size_t populateINodesIn(disk_block * blockPtr, size_t iNodeNum);
extern size_t initINodeListBlock(disk_block * blockPtr, size_t *iNodeList, size_t iNodeAddressesPerBlock);

typedef struct INode {
    // assuming there are more than 1 file systems
    uint32_t device_number;

    // unique to each filesystem
    uint32_t inode_number;

    // file/directory/symlink
    iNodeType type;

    // number of hard links to the file
    uint32_t nlink;

    uint32_t owner_uid;

    // ID of the group owner of the file
    uint32_t gid;

    // size of the file in bytes
    uint32_t size;

    // number of blocks allocated to the file
    uint32_t num_blocks;

    // inode allocated/needs to be written
    uint16_t status;

    // pointers to disk blocks containing data
    disk_block data_blocks[BLOCKS_IN_INODE];

    // file's last access timestamp
    time_t access_time;

    // file creation time(not returned in the inode stat struct)
    time_t birth_time;

    // file's last modified timestamp(timestamp of a directory is changed by the creation
    // or deletion of files in that directory)
    time_t modified_time;
} INode;

#endif // INODE_H

