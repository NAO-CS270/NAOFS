#include "inode/inCoreiNode.h"

void insertDiskInodeData(iNode* disk_inode, inCoreiNode* in_core) {
	in_core->device_number = disk_inode->device_number;
	in_core->inode_number = disk_inode->inode_number;
	in_core->access = disk_inode->access;
	in_core->modification = disk_inode->modification;
	in_core->status_change = disk_inode->status_change;
	in_core->linksCount = disk_inode->linksCount;
	in_core->file_mode = disk_inode->file_mode;
	in_core->owner_uid = disk_inode->owner_uid;
	in_core->group_uid = disk_inode->group_uid;
	in_core->size = disk_inode->size;

	int index = 0;
	while (index < BLOCKS_IN_INODE) {
		in_core->dataBlockNums[index] = disk_inode->dataBlockNums[index];
		index++;
	}
}

void getDiskINodeData(inCoreiNode* in_core, iNode* disk_inode) {
	disk_inode->device_number = in_core->device_number;
	disk_inode->inode_number = in_core->inode_number;
	disk_inode->access = in_core->access;
	disk_inode->modification = in_core->modification;
	disk_inode->status_change = in_core->status_change;
	disk_inode->linksCount = in_core->linksCount;
	disk_inode->file_mode = in_core->file_mode;
	disk_inode->owner_uid = in_core->owner_uid;
	disk_inode->group_uid = in_core->group_uid;
	disk_inode->size = in_core->size;

	int index = 0;
	while (index < BLOCKS_IN_INODE) {
		disk_inode->dataBlockNums[index] = in_core->dataBlockNums[index];
		index++;
	}
}

