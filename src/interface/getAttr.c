#include "interface/getAttr.h"
#include "trav/namei.h"
#include "inode/inCoreiNode.h"
#include "inode/iNode.h"
#include "incoreInodeOps/iget.h"
#include "incoreInodeOps/iput.h"

#include <errno.h>

int attrPopulate(const char *path, struct stat *stbuf) {
	inCoreiNode* inode = getFileINode(path, strlen(path));

    if (inode == NULL) {
		printf("File not found !!\n");
        return -ENOENT;
	}
    
	/*
	stbuf -> st_atime = inode -> access_time;
    stbuf -> st_mtime = inode -> modified_time;
    stbuf -> st_uid = (uid_t) inode -> owner_uid;
    stbuf -> st_gid = (gid_t) inode -> group_uid;
	stbuf -> st_nlink = inode -> linksCount;
	*/
    
	if (inode->type == T_DIRECTORY) {
		stbuf -> st_ctime = inode -> creation_time;
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else if(inode->type == T_REGULAR) {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = inode->size;
	}

	iput(inode);
	return 0;
}

