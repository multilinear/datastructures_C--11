#!/bin/bash 
# run the tests
for x in $(seq 5 5 400); do echo ARITY: ${x}; g++ -O3 -std=c++11 -D ARITY=${x} btree_benchmark.cpp && time ./a.out; done > btree_arity_test.txt 2>&1
# prep the data for graphing
cat btree_arity_test.txt | awk '/ARITY/{print $2} /user/{print $2}' | tr '\n' ' ' | sed -e 's/0m//g' | sed -e 's/s/\n/g' > data
gnuplot -e 'plot [0:300] "foo" with lines; pause -1'
