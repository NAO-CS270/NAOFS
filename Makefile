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
# *********************************************************
# Phony Targets
.PHONY: clean

# Project Targets
nao:
	gcc -o nao temp.c `pkg-config fuse --cflags --libs`

mkfs: $(mkfssrc)mkfs.c $(inodesrc)iNode.h $(mkfssrc)freeBlockList.h $(blksrc)blkfetch.h $(paramssrc)params.h freeBlockList inode| $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)mkfs

freeBlockList: $(mkfssrc)freeBlockList.c $(mkfssrc)freeBlockList.h $(blksrc)mdisk.h $(paramssrc)params.h | $(mkfsbld)
	$(CC) $(CFLAGS) -c $< -o $(mkfsbld)freeBlockList.o

inode: $(inodesrc)iNode.c $(inodesrc)iNode.h | $(inodebld)
	$(CC) $(CFLAGS) -c $< -o $(inodebld)inode.o

fetcher: $(blksrc)blkfetch.c $(blksrc)blkfetch.h $(blksrc)mdisk.h mdisk | $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)fetcher.o

mdisk: $(blksrc)mdisk.c $(blksrc)mdisk.h | $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)mdisk.o

$(mkfsbld):
	mkdir -p $@

$(inodebld):
	mkdir -p $@

$(blkbld):
	mkdir -p $@

clean:
	rm -rf build nao
