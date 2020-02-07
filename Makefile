# Makefile for NAOFS
#
# *********************************************************
# Parameters to control Makefile operation
CC = gcc

INC_DIR = src/
CFLAGS = -Wall -pedantic -I $(INC_DIR)

# Path and file variables
blksrc = src/dsk/
blkbld = build/dsk/

mkfssrc = src/mkfs/
mkfsbld = build/mkfs/

paramssrc = src/mandsk/

inodesrc = src/inode/
inodebld = build/inode/

incoreInodeOpssrc = src/incoreInodeOps/
incoreInodeOpsbld = build/incoreInodeOps/
# *********************************************************
# Phony Targets
.PHONY: clean

# Project Targets
nao:
	gcc -o nao temp.c `pkg-config fuse --cflags --libs`

mkfs: $(mkfssrc)mkfs.c $(inodesrc)iNode.h $(mkfssrc)freeBlockList.h $(mkfssrc)metaBlocks.h $(blksrc)blkfetch.h $(paramssrc)params.h freeBlockList inode| $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)mkfs

freeBlockList: $(mkfssrc)freeBlockList.c $(mkfssrc)freeBlockList.h $(blksrc)mdisk.h $(paramssrc)params.h | $(mkfsbld)
	$(CC) $(CFLAGS) -c $< -o $(mkfsbld)freeBlockList.o

inode: $(inodesrc)iNode.c $(inodesrc)iNode.h | $(inodebld)
	$(CC) $(CFLAGS) -c $< -o $(inodebld)inode.o

fetcher: $(blksrc)blkfetch.c $(blksrc)blkfetch.h $(blksrc)mdisk.h mdisk | $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)fetcher.o

mdisk: $(blksrc)mdisk.c $(blksrc)mdisk.h | $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)mdisk.o

freeList: $(incoreInodeOpssrc)freeList.c $(incoreInodeOpssrc)freeList.h $(incoreInodeOpssrc)node.h | $(incoreInodeOpsbld)
	$(CC) $(CFLAGS) -c $< -o $(incoreInodeOpsbld)freeList.o

hashQ: $(incoreInodeOpssrc)hashQ.c $(incoreInodeOpssrc)hashQ.h $(incoreInodeOpssrc)node.h $(paramssrc)params.h | $(incoreInodeOpsbld)
	$(CC) $(CFLAGS) -c $< -o $(incoreInodeOpsbld)hashQ.o

iget: $(incoreInodeOpssrc)iget.c $(incoreInodeOpssrc)iget.h $(blksrc)blkfetch.h $(blksrc)mdisk.h $(incoreInodeOpssrc)hashQ.h $(incoreInodeOpssrc)freeList.h $(incoreInodeOpssrc)node.h $(inodesrc)inCoreiNode.h $(inodesrc)iNode.h $(paramssrc)params.h  | $(incoreInodeOpsbld)
	$(CC) $(CFLAGS) -c $< -o $(incoreInodeOpsbld)iget.o

iput: $(incoreInodeOpssrc)iput.c $(incoreInodeOpssrc)iput.h $(blksrc)blkfetch.h $(blksrc)mdisk.h $(incoreInodeOpssrc)hashQ.h $(incoreInodeOpssrc)freeList.h $(incoreInodeOpssrc)node.h $(inodesrc)inCoreiNode.h $(paramssrc)params.h | $(incoreInodeOpsbld)
	$(CC) $(CFLAGS) -c $< -o $(incoreInodeOpsbld)iput.o

$(mkfsbld):
	mkdir -p $@

$(inodebld):
	mkdir -p $@

$(blkbld):
	mkdir -p $@

$(incoreInodeOpsbld):
	mkdir -p $@

clean:
	rm -rf build nao
