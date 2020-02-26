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

. ../shunit2-2.1.6/src/shunit2