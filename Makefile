# Makefile for NAOFS
#
# *********************************************************
# Parameters to control Makefile operation
CC = gcc
CFLAGS = -Wall -pedantic

# Path and file variables
blksrc = src/dsk/
blkbld = build/dsk/

# *********************************************************
# Phony Targets
.PHONY: clean

# Project Targets
nao:
	gcc -o nao temp.c `pkg-config fuse --cflags --libs`

blockFetch: $(blkbld)mdisk.o | $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $(blkbld)$@

$(blkbld)mdisk.o: $(blksrc)mdisk.c $(blksrc)mdisk.h | $(blkbld)
	$(CC) $(CFLAGS) -c $< -o $@

$(blkbld):
	mkdir -p $@

clean:
	rm -rf build nao
