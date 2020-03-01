#include "interface/unlink.h"
#include "interface/create.h"
#include "inode/inCoreiNode.h"
#include "inode/iNode.h"
#include "trav/namei.h"
#include "incoreInodeOps/iput.h"
#include "trav/directory.h"

#include <string.h>

int unlinkFile(const char* file, struct fuse_context* fuseContext) {
    char *filename = (char *)malloc((pathLen + 1)*sizeof(char));
    inCoreiNode* parentInode = validateThenGetParentINode(T_REGULAR, file, filename);

    /**
     * search for the directory entry in teh parent dir
     * save the pointer of that entry. So that we dont have to read from the disk again
     * bmap - get the last directory entry
     * memcpy into the new location
     * write the mempy-ed disk block into the disk
     * iget on the unlinked inode number.
    // the book says that if unlinking the current directory... */
    // if (last component of file name is ".")
    // increment mode reference count;

    // TODO: if file is DIR but user is not super user, return error(refer book)



    //write parent directory: zero inode number of unlinked file; release inode parent directory (algorithm iput);

    inode->linksCount--;
    iput(inode);
    iput(parentInode);
}