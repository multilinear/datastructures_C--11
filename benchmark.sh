set -x

total=100000000
size=2;
iterations=100
rm log
while [[ ${iterations} != 0 ]]; do 
  size=$((size+size))
  iterations=$((total/size))
  export TEST_ITERATIONS=${iterations}
  export TEST_SIZE=${size}
  make clean
  TEST_ITERATIONS=${iterations} TEST_SIZE=${size} make -e dicts_benchmark &>> log
done
