#!/usr/bin/env bash

# get the file system source code
git clone https://github.com/NAO-CS270/NAOFS.git

# build
cd NAOFS
cmake -Bbuild -H.
cd build
make

# start the FUSE file system daemon
mkdir mount_dir
./naofs ./mount_dir

fusermount -u ./mount_dir -o default_permissions

# enter the file system and now execute the file system calls
# TODO: Mention the system calls which are working at present
cd mount_dir