#!/bin/sh

testChangeDir() {
  pwd1=$(pwd)
  cd fsRoot || fail "cd-in error"
  cd ../ || fail "cd-back error"
  pwd2=$(pwd)
  assertEquals "$pwd1" "$pwd2"
}

testCreateDir() {
  cd fsRoot || fail "cd-in error"
  mkdir test || fail "mkdir error"
  if [ ! -d test ]; then
    # Control will enter here if $DIRECTORY doesn't exist.
    fail "dir create error"
  fi
  assertEquals "$pwd1" "$pwd2"
}

testPresentWorkingDir() {
  pwd=$(pwd)
  assertEquals "/home/travis/build/NAO-CS270/NAOFS/build/fsRoot" "$pwd"
}

testListDir() {
  lsResult=$(ls)
  assertEquals "test" "$lsResult"
}

testCdToCreatedDir() {
  pwd1=$(pwd)
  cd test || fail "cd to created dir error"
  pwd2=$(pwd)
  cd ../ || fail "cd-back error"
  assertEquals "$pwd1/test" "$pwd2"
}

testWriteAndRead() {
  pwd1=$(pwd)
  touch a
  echo 1 > a
  catResult=$(cat a)
  assertEquals "1" "$catResult"
}

testDownloadFileSmall() {
  wget http://178.128.139.251:9000/512
  md5Output=$(md5sum 512)
  md5Hash=${md5Output%% *}

  assertEquals "bf619eac0cdf3f68d496ea9344137e8b" "$md5Hash"
}

testDownloadFileOneFullBlock() {
  wget http://178.128.139.251:9000/1024
  md5Output=$(md5sum 1024)
  md5Hash=${md5Output%% *}

  assertEquals "0f343b0931126a20f133d67c2b018a3b" "$md5Hash"
}

testDownloadFileMoreThanOneBlock() {
  wget http://178.128.139.251:9000/1100
  md5Output=$(md5sum 1100)
  md5Hash=${md5Output%% *}

  assertEquals "1f8dc9e63e9bf7f1ed20b99b444945aa" "$md5Hash"
}

testDownloadFileLargeFile() {
  wget http://178.128.139.251:9000/60M
  md5Output=$(md5sum 60M)
  md5Hash=${md5Output%% *}

  assertEquals "76c136bc0d680abd978d9934070c01e5" "$md5Hash"
}

. ../shunit2-2.1.6/src/shunit2