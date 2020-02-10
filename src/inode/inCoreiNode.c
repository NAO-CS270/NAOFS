#include "inCoreiNode.h"

void insertDiskInodeData(iNode* disk_inode, inCoreiNode* in_core) {
	in_core->device_number = disk_inode->device_number;
	in_core->inode_number = disk_inode->inode_number;
	in_core->modified_time = disk_inode->modified_time;
	in_core->access_time = disk_inode->access_time;
	in_core->creation_time = disk_inode->creation_time;
	in_core->linksCount = disk_inode->linksCount;
	in_core->type = disk_inode->type;
	in_core->mode = disk_inode->mode;
	in_core->owner_uid = disk_inode->owner_uid;
	in_core->group_uid = disk_inode->group_uid;

	int index = 0;
	while (index < BLOCKS_IN_INODE) {
		in_core->dataBlockNums[index] = disk_inode->dataBlockNums[index];
		index++;
	}
}