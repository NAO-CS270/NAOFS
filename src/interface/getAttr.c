#include "interface/getAttr.h"
#include "trav/namei.h"
#include "inode/inCoreiNode.h"
#include "inode/iNode.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int attrPopulate(const char *path, struct stat *stbuf) {
	inCoreiNode* inode = getFileINode(path, strlen(path));

    if (inode == NULL) {
		printf("File not found !!\n");
        return -ENOENT;
	}
    
	/*
    stbuf -> st_uid = (uid_t) inode -> owner_uid;
    stbuf -> st_gid = (gid_t) inode -> group_uid;
	stbuf -> st_nlink = inode -> linksCount;
	if (S_ISDIR(inode->file_mode)) {
		stbuf->st_nlink = 2;
	}
	else {
		stbuf->st_nlink = 1;
	}
	*/
	stbuf -> st_atime = inode -> access;
	stbuf -> st_mtime = inode -> modification;
	stbuf -> st_ctime = inode -> status_change;

	stbuf -> st_mode = inode -> file_mode;
	stbuf -> st_size = inode -> size;
    stbuf -> st_uid = inode -> owner_uid;
    stbuf -> st_gid = inode -> group_uid;

	iput(inode);
	return 0;
}

int changeMode(const char* path, mode_t mode) {
    inCoreiNode* inode = getFileINode(path, strlen(path));
    if (inode == NULL) {
        printf("[changeMode] File not found !!\n");
        return -ENOENT;
    }

    pthread_mutex_lock(&(inode->iNodeMutex));
    if (inode->type == T_DIRECTORY) {
        mode = S_IFDIR | mode;
    }
    inode -> file_mode = mode;
    inode -> inode_changed = true;
    pthread_mutex_unlock(&(inode->iNodeMutex));
    iput(inode);

    return 0;
}
