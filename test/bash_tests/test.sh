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
  assertEquals "$pwd" "/home/travis/build/NAO-CS270/NAOFS/build/fsRoot"
}

testListDir() {
  lsResult=$(ls)
  assertEquals "$lsResult" "test"
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
  assertEquals "$catResult" "1"
}

testDownloadFileSmall() {
  wget http://178.128.139.251:9000/512
  md5Output=$(md5sum 512)
  md5Hash=${md5Output%% *}

  assertEquals "$md5Hash" "bf619eac0cdf3f68d496ea9344137e8b"
}

testDownloadFileOneFullBlock() {
  wget http://178.128.139.251:9000/1024
  md5Output=$(md5sum 1024)
  md5Hash=${md5Output%% *}

  assertEquals "$md5Hash" "0f343b0931126a20f133d67c2b018a3b"
}

testDownloadFileMoreThanOneBlock() {
  wget http://178.128.139.251:9000/1100
  md5Output=$(md5sum 1100)
  md5Hash=${md5Output%% *}

  assertEquals "$md5Hash" "1f8dc9e63e9bf7f1ed20b99b444945aa"
}

testDownloadFileLargeFile() {
  wget http://178.128.139.251:9000/60M
  md5Output=$(md5sum 60M)
  md5Hash=${md5Output%% *}

  assertEquals "$md5Hash" "76c136bc0d680abd978d9934070c01e5"
}

. ../shunit2-2.1.6/src/shunit2