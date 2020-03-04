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
#  rm testFile
}

testWriteAndReadCreatedFile() {
  pwd1=$(pwd)
  touch testFile2
  echo 1 > testFile2
  catResult=$(cat testFile2)
  assertEquals 'write with touch failed' "1" "$catResult"
#  rm testFile2
}

testDownloadFile() {
  wget https://s.yimg.com/rz/p/yahoo_frontpage_en-US_s_f_p_205x58_frontpage_2x.png -O testDownload
  md5Result=$(md5sum testDownload | awk '{print $1;}')

  assertEquals 'Downloaded File not matched' "d798d33d3961f3d8fe0410ede7e6dc77" "$md5Result"
#  rm testDownload
}

testFileOneFullBlock() {
  N=1023
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  echo "$content" > testFile1023
  catResult=$(cat testFile1023)

  assertEquals '1K - 1 not matched' "$content" "$catResult"
#  rm testFile1023
}

testFileMoreThanOneBlock() {
  N=1024
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  echo "$content" > testFile1024
  catResult=$(cat testFile1024)

  assertEquals '1K not matched' "$content" "$catResult"
#  rm testFile1024
}

testFileFirstIndirectBlock() {
  N=$((10*1024 - 1))
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  echo "$content" > testFileFirstIndirect
  catResult=$(cat testFileFirstIndirect)

  assertEquals 'First indirect block not matched' "$content" "$catResult"
#  rm testFileFirstIndirect
}

testFileFirstIndirectBlock() {
  N=$((522*1024 - 2))
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  echo "$content" > testFileLastIndirect
  catResult=$(cat testFileLastIndirect)

  assertEquals 'Last indirect block not matched' "$content" "$catResult"
#  rm testFileLastIndirect
}

testFileFirstDoubleIndirectBlock() {
  N=$((522*1024 - 1))
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  echo "$content" > testFileFirstDoubleIndirect
  catResult=$(cat testFileFirstDoubleIndirect)

  assertEquals 'First double-indirect block not matched' "$content" "$catResult"
#  rm testFileFirstDoubleIndirect
}

testLargeFile() {
  N=$((60*1024)) # 60Mb
  content=$(seq 1 $N | sed 's/.*/./' | tr -d '\n')

  echo "$content" > testFile60M
  catResult=$(cat testFile60M)

  assertEquals '60M not matched' "$content" "$catResult"
#  rm testFile60M
}

. ../shunit2-2.1.6/src/shunit2