#!/bin/sh

testChangeDir() {
  pwd1=$(pwd)
  cd fsRoot || fail "cd in error"
  cd ../ || fail "cd back error"
  pwd2=$(pwd)

  assertEquals "$pwd1" "$pwd2"
}

testEquality() {
	assertEquals 1 1
}

. ../shunit2-2.1.6/src/shunit2