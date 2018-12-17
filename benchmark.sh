#!/bin/bash
set -x
target=$1 

total=134217728
size=2;
iterations=$((total/size))
rm log
while [[ ${iterations} != 0 ]]; do 
  export TEST_ITERATIONS=${iterations}
  export TEST_SIZE=${size}
  make clean
  TEST_ITERATIONS=${iterations} TEST_SIZE=${size} make -e ${1} &>> log
  size=$((size+size))
  iterations=$((total/size))
done
