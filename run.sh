#! /bin/sh

cmake -Bbuild -H.
cd build
make

mkdir fsRoot
gnome-terminal -x bash -c "./naofs fsRoot -d -f -s"
cd fsRoot/
