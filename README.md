# NAOFS [![Build Status](https://travis-ci.org/NAO-CS270/NAOFS.svg?branch=master)](https://travis-ci.org/NAO-CS270/NAOFS) [![Coverage Status](https://coveralls.io/repos/github/NAO-CS270/NAOFS/badge.svg)](https://coveralls.io/github/NAO-CS270/NAOFS)
![](images/nao.png)


Not Any Other File System - Graduate Operating Systems Class Project. Building a linux file system using FUSE

### Requirements
Install FUSE development package (needed for compilation) -
`apt-get install libfuse-dev` (for Debian-based)

Install `pkg-config` -
This lets you pass information regarding libraries needed by gcc for compilation.

### Status
FUSE callbacks implemented (not correctly, ofcourse) -
```
getattr
readdir
read
```

### Running
Compile the project -
```
cmake -Bbuild -H.
cd build/
make
```
* The file system can be run in-memory or be backed by a disk.
* Size of the file system can be set in `include/mandsk/params.h`.
* Make a directory to mount the file system in - `mkdir -p ${FILESYSTEM_ROOT}`.
* Run the filesystem - `./naofs -s ${FILESYSTEM_ROOT}`.
* The following options can be given -
	* `-f` to run the filesystem in foreground (prints a lot of logs).
	* `-d` to enable debug logs (TODO - move the logs printed by above option into this).
	* `-o blkdev -o fsname=${DEVICE_FILENAME}` to run the file system on the physical disk `DEVICE_NAME`.

To run the file system on a physical disk, `mkfs` needs to be done on the disk before running our file system -
```
cd ${PROJECT_ROOT}/build/src/mkfs
./mkfs #{DEVICE_FILENAME}
```
This would take some time depending on the size of the disk configuired in the project.

To automate all of these steps, we have also provided a script -
* To setup dependencies, do `./run.sh setup ${DEVICE_FILENAME}`
* To build, do `./run.sh build`.
* For mkfs, do `./run.sh mkfs ${DEVICE_FILENAME}`.
* To run the filesystem, do `./run.sh nologs`.
* To clean the project, do `./run.sh clean`.
* To do all these steps in one go, do `./run.sh present ${DEVICE_FILENAME}`

