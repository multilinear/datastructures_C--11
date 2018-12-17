#!/bin/bash
set -x
target=$1 

total=134217728
size=2;
iterations=1
rm log
while [[ ${iterations} != 0 ]]; do 
  size=$((size+size))
  iterations=$((total/size))
  export TEST_ITERATIONS=${iterations}
  export TEST_SIZE=${size}
  for ((arity=2;arity<9;arity++)); do
    make clean
    TEST_ITERATIONS=${iterations} TEST_SIZE=${size} RADIX=${arity} make -e ${1} &>> log
  done
done
