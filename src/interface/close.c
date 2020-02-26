#include "interface/close.h"
#include "fdTable/fileTables.h"
#include "inode/inCoreiNode.h"
#include "incoreInodeOps/iput.h"
#include "utils/utils.h"

#include <fuse.h>
#include <stdbool.h>

int closeFile(struct fuse_file_info *fi, struct fuse_context *fuse_context) {
	int retVal = 0;
	fileTableEntry *file = getFileDescriptor(fuse_context->pid, fi->fh, &retVal);
	if (retVal != 0) {
		debug_print("Some error for fd: %d\n", fi->fh);
		return retVal;
	}

	file->validEntry = false;
	return iput(file->inode);
}
