#! /bin/sh

set -e

BASEDIR=$(realpath $0)
BASEDIR=$(dirname $BASEDIR)

DEVICE_FILE="/dev/vdc1"

GETIN="cd $BASEDIR/build"

GETTOMKFS=""
MKFS=""

MKROOT="mkdir -p $BASEDIR/build/fsRoot"
NAOFS="./naofs -d -f -s fsRoot"

MAKEGEN=""
MAKE="make"

UNMOUNT=""
CLEAN=""

case "$1" in
	rebuild)
		MAKEGEN="cmake -Bbuild -H."
		MKROOT=""
		NAOFS=""
		;;
	nomake)
		MAKE=""
		NAOFS="./naofs -d -f -s -o blkdev -o fsname=$DEVICE_FILE"
		;;
	ondisk)
		NAOFS="./naofs -d -f -s -o blkdev -o fsname=$DEVICE_FILE"
		;;
	diskinit)
		GETTOMKFS="cd $BASEDIR/build/src/mkfs"
		MKFS="./mkfs $DEVICE_FILE"
		MKROOT=""
		NAOFS=""
		;;
	clean)
		MAKE=""
		UNMOUNT="fusermount -u $BASEDIR/build/fsRoot"
		CLEAN="rm -rf $BASEDIR/build"
		MKROOT=""
		NAOFS=""
		;;
esac

$MAKEGEN
$GETIN
$MAKE

$GETTOMKFS
$MKFS



$GETIN
$MKROOT
$NAOFS

$UNMOUNT
$CLEAN

cd $BASEDIR

