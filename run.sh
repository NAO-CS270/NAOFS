#! /bin/sh

set -e

BASEDIR=$(realpath $0)
BASEDIR=$(dirname $BASEDIR)

DEVICE_FILE="/dev/vdc1"
DEVICE_FILE=$2

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
    present)
        MAKEGEN="cmake -Bbuild -H."
        GETTOMKFS="cd $BASEDIR/build/src/mkfs"
        MKFS="./mkfs $DEVICE_FILE"
        NAOFS="./naofs -s -o blkdev -o fsname=$DEVICE_FILE -o allow_other,default_permissions ./fsRoot"
        ;;
    setup)
        sudo apt-get install software-properties-common -y
        sudo add-apt-repository ppa:george-edison55/cmake-3.x
        sudo apt-get update
        sudo apt-get install cmake g++ pkg-config libfuse-dev -y
        parted $DEVICE_FILE mklabel gpt
        parted -a optimal $DEVICE_FILE mkpart primary 0% 100%
        GETIN=""
        MAKE=""
        MKROOT=""
        NAOFS=""
        ;;
	build)
		MAKEGEN="cmake -Bbuild -H."
		NAOFS=""
		;;
	mkfs)
		MAKE=""
		GETTOMKFS="cd $BASEDIR/build/src/mkfs"
		MKFS="./mkfs $DEVICE_FILE"
		MKROOT=""
		NAOFS=""
		;;
	nologs)
		MAKE=""
		MKROOT=""
		NAOFS="./naofs -s -o blkdev -o fsname=$DEVICE_FILE -o allow_other,default_permissions ./fsRoot"
		;;
	debug)
		MAKE=""
		MKROOT=""
		NAOFS="./naofs -d -f -s -o blkdev -o fsname=$DEVICE_FILE -o allow_other,default_permissions ./fsRoot"
		;;
	clean)
		MAKE=""
		UNMOUNT="umount $BASEDIR/build/fsRoot"
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

