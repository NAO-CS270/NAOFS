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
  echo 1 > testFile
  catResult=$(cat testFile)
  assertEquals 'write without touch failed' "1" "$catResult"
  rm testFile
}

testWriteAndReadCreatedFile() {
  pwd1=$(pwd)
  touch testFile
  echo 1 > testFile
  catResult=$(cat testFile)
  assertEquals 'write with touch failed' "1" "$catResult"
  rm testFile
}

testDownloadFileSmall() {
  N=512
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  wget http://178.128.139.251:9000/512 -O testFile
  catResult=$(cat testFile)

  assertEquals '512B not matched' "$content" "$catResult"
  rm testFile
}

#testFileOneFullBlock() {
#  N=1024
#  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')
#
#  echo "$content" > testFile
#  catResult=$(cat testFile)
#
#  assertEquals '1K not matched' "$content" "$catResult"
#  rm testFile
#}

#testFileMoreThanOneBlock() {
#  N=1100
#  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')
#
#  echo "$content" > testFile
#  catResult=$(cat testFile)
#
#  assertEquals '1100B not matched' "$content" "$catResult"
#  rm testFile
#}

#testDownloadFileLargeFile() {
#  N=61440 # 60Mb
#  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')
#
#  echo "$content" > testFile
#  catResult=$(cat testFile)
#
#  assertEquals '60M not matched' "$content" "$catResult"
#  rm testFile
#}

. ../shunit2-2.1.6/src/shunit2