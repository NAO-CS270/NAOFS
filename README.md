# NAOFS [![Build Status](https://travis-ci.org/NAO-CS270/NAOFS.svg?branch=master)](https://travis-ci.org/NAO-CS270/NAOFS) [![Coverage Status](https://coveralls.io/repos/github/NAO-CS270/NAOFS/badge.svg?branch=master)](https://coveralls.io/github/NAO-CS270/NAOFS?branch=master)


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
Compile the project - `make nao`
Mount in random local directory - `./nao tempdir`
Go into the directory and test commands.

### Clean
`make clean`
