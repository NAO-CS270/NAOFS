#!/usr/bin/env bash

sudo apt-get install software-properties-common -y
sudo add-apt-repository ppa:george-edison55/cmake-3.x
sudo apt-get update
sudo apt-get install cmake g++ pkg-config libfuse-dev -y


# get the file system source code
git clone https://github.com/NAO-CS270/NAOFS.git

# build
cd NAOFS
cmake -Bbuild -H.
cd build
make

# start the FUSE file system daemon
mkdir fsRoot
./naofs -f -s -d -o allow_other,default_permissions fsRoot

# enter the file system and now execute the file system calls
# TODO: Mention the system calls which are working at present
cd mount_dir