#! /bin/sh

cmake -Bbuild -H.
cd build
make
./naofs
cd ../
rm -rf build
