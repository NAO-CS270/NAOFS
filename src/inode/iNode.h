#ifndef INODE_H
#define INODE_H

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "../dsk/mdisk.h"

#define BLOCKS_IN_INODE 13

enum iNodeType {
	T_FREE		= 0,
	T_REGULAR	= 1,
	T_DIRECTORY	= 2
};
typedef enum iNodeType iNodeType;

enum iNodeMode {
    P_SUID = 04000,  // set-user-ID bit (see execve(2))
    P_SGID = 02000,  // set-group-ID bit
    P_SVTX = 01000,  // sticky bit

    P_RUSR = 00400,  // owner has read permission
    P_WUSR = 00200,  // owner has write permission
    P_XUSR = 00100,  // owner has execute permission

    P_RGRP = 00040,  // group has read permission
    P_WGRP = 00020,  // group has write permission
    P_XGRP = 00010,  // group has execute permission

    P_ROTH = 00004,  // others have read permission
    P_WOTH = 00002,  // others have write permission
    P_XOTH = 00001,  // others have execute permission
};
typedef enum iNodeMode iNodeMode;

// Using this struct in the project. Keeping the old for for reference.
struct iNode {
	size_t device_number;

	size_t inode_number; 

	size_t linksCount;

	iNodeType type;

	iNodeMode mode;

	size_t owner_uid;			// TODO - Have to figure out how to initialize this.

	size_t group_uid;			// TODO - Have to figure out how to initialize this.

	size_t dataBlockNums[BLOCKS_IN_INODE];
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

    // permissions for the file
    iNodeMode mode;
} INode;


#endif // INODE_H
