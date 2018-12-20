#define ARRAY_DEBUG

#include <cstdint>
#include <stdio.h>
#include "panic.h"
#include "array.h"
#include "medianfind.h"
#include "sort.h"

class IntCompare {
  public:
    static int32_t compare(uint32_t v1, uint32_t v2) {
      return v1 - v2;
    }
};

void print_array(Array<uint32_t> *a) {
  printf("[");
  for (size_t i=0; i<a->size(); i++) {
    printf("%d,", (*a)[i]);
  }
  printf("]\n");
}

void run(Array<uint32_t>* input, size_t k){
  Array<uint32_t> sorted(input);  
  Array<uint32_t> copy(input);  
  Array<uint32_t> tmp(input->size());  
  fast_sort<Array<uint32_t>,Array<uint32_t>>(&sorted, &tmp);
  size_t ind2 = findkth<Array<uint32_t>, IntCompare, false>(&copy, k);
  size_t ind1 = findkth<Array<uint32_t>, IntCompare, true>(&copy, k);
  // Verify it's the median
  if (sorted[k] != (*input)[ind2]) {
    print_array(input);
    print_array(&sorted);
    printf("findkth returned %lu\n", ind2); 
    printf("sorted[%lu]=%u but input[findkth(k=%lu)]=%u\n", k, sorted[k], k, (*input)[ind2]);
    PANIC("findkth (linear=false) returned wrong index\n");
  }
  if (sorted[k] != (*input)[ind1]) {
    print_array(input);
    print_array(&sorted);
    printf("findkth returned %lu\n", ind1); 
    printf("sorted[%lu]=%u but input[findkth(k=%lu)]=%u\n", k, sorted[k], k, (*input)[ind1]);
    PANIC("findkth (linear=true) returned wrong index\n");
  }
  // Verify we didn't mess with the array
  for (size_t i=0; i<input->size(); i++) {
    if ((*input)[i] != copy[i]) {
      print_array(input);
      print_array(&copy);
      PANIC("findkth modified array. Input array (first) != copy (second)")
    }
  }
}

template<typename ArrayType, typename DataType>
void permutation_helper(ArrayType* a, void (*callback)(ArrayType*, DataType), size_t p, DataType opaque_data) {
  size_t i;
  // When we reach the end, we're done
  if (p+1 >= a->size()) {
    callback(a, opaque_data);
    return;
  }
  // No permutation, just call
  permutation_helper(a, callback, p+1, opaque_data);
  // Then swap with each other option, and call
  for (i=p+1; i<a->size(); i++) {
    a->swap(p,i);
    permutation_helper(a, callback, p+1, opaque_data);
    a->swap(p,i);
  }
}

template<typename ArrayType, typename DataType>
void permutations(Array<uint32_t>* a, void (*callback)(ArrayType*, DataType), DataType opaque_data) {
  permutation_helper<ArrayType>(a, callback, 0, opaque_data);
}

int main(){
  printf("Begin MedianFind.h unittest\n");
  Array<uint32_t> testdata; 
  // Exhaustive testing of up to 8 elements
  // For findkth we're not interested in 0 length arrays
  for (int x=1; x<10; x++) {
    testdata.resize(x);
    if (x>0) {
      testdata[x-1] = x;
    }
    permutations<Array<uint32_t>, size_t>(&testdata, run, testdata.size()/2); 
  }
  // Larger test on easier to work with numbers
  for (uint32_t x=1; x<20; x++) {
    testdata.resize(x);
    if (x>0) {
      testdata[x-1] = x;
    }
    run(&testdata, testdata.size()/2);
  }
  for (uint32_t x=1; x<20; x++) {
    testdata.resize(x);
    if (x>0) {
      testdata[x-1] = 20-x;
    }
    run(&testdata, testdata.size()/2);
  }
  // Random testing of large arrays
  for (uint32_t i=1000; i<1500; i+=1) {
    testdata.resize(i);
    for (uint32_t x=0; x<i; x++) {
      testdata[x] = rand();
    }
    run(&testdata, testdata.size()/2);
  }
  // Test that findkth_pivot_helper is giving a good approximation
  // This is necessary to ensure that we get linear performance from our
  // linear quickselect algorithm
  Array<size_t> ind;
  Array<uint32_t> sorted;
  Array<uint32_t> tmp;
  for(uint32_t i=10; i<1000; i++) {
    testdata.resize(i);
    for (uint32_t x=0; x<i; x++) {
      testdata[x] = rand();
    }
    // Okay, run the helper
    ind.resize(i);
    for (uint32_t k=0; k<i; k++) {
      ind[k]=k;
    }
    uint32_t median = testdata[ind[findkth_pivot_helper<Array<uint32_t>, IntCompare>(&testdata, &ind, 0, i-1)]];
    // now calculate where that lands in the list
    array_copy<Array<uint32_t>,Array<uint32_t>>(&sorted, &testdata);
    tmp.resize(i);
    fast_sort<Array<uint32_t>, Array<uint32_t>>(&sorted, &tmp);
    uint32_t q=0;
    while (sorted[q] != median) {
      q++;
    }
    if (q < (i-1)*0.3 || q > (i-1)*0.7) {
      printf("q=%u, i=%u\n", q, i);
      PANIC("median approximation is bad");
    }
  }
  printf("PASS\n");
  return 0;
}
