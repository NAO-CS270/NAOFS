# NAOFS
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
Compile the project - `make`
Mount in random local directory - `./nao tempdir`
Go into the directory and test commands.

### Clean
`make clean`

### Run it using the below command
gcc -o nao temp.c pkg-config fuse --cflags --libs
