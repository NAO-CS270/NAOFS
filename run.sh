#! /bin/sh

fusermount -u /home/dimone/workplace/NAOFS/build/fsRoot
rm -rf build
cmake -Bbuild -H.
cd build
make

mkdir fsRoot
gnome-terminal -e "bash -c 'gdb naofs'"
#gnome-terminal -x bash -c "./naofs fsRoot -d -f -s"

