#!/bin/bash
set -x
target=$1 

total=134217728
size=4;
iterations=$((total/size))
rm log
while [[ ${iterations} != 0 ]]; do 
  export TEST_ITERATIONS=${iterations}
  export TEST_SIZE=${size}
  for ((arity=5;arity<2000;arity*=2)); do
    make clean
    TEST_ITERATIONS=${iterations} TEST_SIZE=${size} BTREE_ARITY=${arity} make -e ${1} &>> log
  done
  size=$((size+size))
  iterations=$((total/size))
done
