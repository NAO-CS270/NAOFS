#include "inode/inCoreiNode.h"
#include "mandsk/params.h"
#include "trav/directory.h"
#include "trav/namei.h"
#include "incoreInodeOps/iput.h"
#include "utils/utils.h"

#include <fuse.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void fillDirectoryEntries(fuse_fill_dir_t filler, void *buffer, inCoreiNode *iNodePtr) {
	size_t entriesInDirectory = (iNodePtr->size) / DIRECTORY_ENTRY_SIZE;
	size_t entriesPerIteration = BLOCK_SIZE / DIRECTORY_ENTRY_SIZE;
	size_t entriesOffset = 0;
	size_t entriesRead;
	size_t counter;

	directoryEn1try *entryBuffer = (directoryEntry *)malloc(entriesPerIteration * sizeof(directoryEntry));

	while (entriesOffset < entriesInDirectory) {
		memset(entryBuffer, 0, entriesPerIteration * sizeof(directoryEntry));
		entriesRead = searchINodeDirectoryEntries(iNodePtr, NULL, entriesOffset, entryBuffer, entriesPerIteration);

		counter = 0;
		while (counter < entriesRead) {
			filler(buffer, (entryBuffer + counter)->name, NULL, 0);
			counter++;
		}
		entriesOffset += entriesRead;
	}
	free(entryBuffer);
}

int readDirectory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    inCoreiNode* iNodeOfPath = getFileINode(path, strlen(path));
    if(iNodeOfPath == NULL) {
        debug_print("iNode not found for path: %s\n", path);
        return -1;
    }

	fillDirectoryEntries(filler, buf, iNodeOfPath);

	iput(iNodeOfPath);
    return 0;
}

