#define ARRAY_DEBUG

#include <cstdint>
#include <stdio.h>
#include "panic.h"
#include "array.h"
#include "sort.h"

class IntCompare {
  public:
    static int32_t compare(uint32_t v1, uint32_t v2) {
      return v1 - v2;
    }
};

void print_array(Array<uint32_t> *a) {
  printf("[");
  for (size_t i=0; i<a->len(); i++) {
    printf("%d,", (*a)[i]);
  }
  printf("]\n");
}

void run(Array<uint32_t>* input, void* _unused){
  Array<uint32_t> *bad = nullptr;
  //printf("Run\n");
  Array<uint32_t> s_a(input);  
  Array<uint32_t> b_a(input);  
  Array<uint32_t> q_a(input);  
  Array<uint32_t> m_a(input);  
  Array<uint32_t> h_a(input);  
  Array<uint32_t> br_a(input);  
  Array<uint32_t> r_a(input);  
  Array<uint32_t> tmp_a(m_a.len());  
  //printf("testing: ");
  //print_array(input);
  //printf("Selection: ");
  selection_sort<Array<uint32_t>,IntCompare>(&s_a);
  //print_array(&s_a);
  //printf("Bubble: ");
  bubble_sort<Array<uint32_t>,IntCompare>(&b_a);
  //print_array(&q_a);
  //printf("Quick: ");
  quick_sort<Array<uint32_t>,IntCompare>(&q_a);
  //printf("Merge Test %lu\n", m_a.len());
  //print_array(&m_a);
  //printf("Merge: ");
  merge_sort<Array<uint32_t>,Array<uint32_t>,IntCompare>(&m_a, &tmp_a);
  //print_array(&m_a);
  heap_sort<Array<uint32_t>,IntCompare>(&h_a);
  //printf("bradix_sort input\n");
  //print_array(&br_a);
  bradix_sort<Array<uint32_t>,uint32_t>(&br_a);
  //printf("bradix_sort output\n");
  radix_sort<Array<uint32_t>,Array<uint32_t>,uint32_t, 5>(&r_a, &tmp_a);
  for (size_t i=0; i<s_a.len(); i++) {
    if (s_a[i] != b_a[i]) {
      bad = &b_a;
    }
    if (s_a[i] != q_a[i]) {
      bad = &q_a;
    }
    if (s_a[i] != m_a[i]) {
      bad = &m_a;
    }
    if (s_a[i] != h_a[i]) {
      bad = &h_a;
    }
    if (s_a[i] != br_a[i]) {
      bad = &br_a;
    }
    if (s_a[i] != r_a[i]) {
      bad = &r_a;
    }
  }
  if (bad) {
    printf("ERROR! failed to sort\n");
    printf("Length was %li\n", input->len());
    printf("Input\n");
    print_array(input);
    printf("Output\n");
    print_array(bad);
    printf("Output should be\n");
    print_array(&s_a);
    PANIC("Nonmatching values");
  }
  //printf("Done run\n");
}


template<typename ArrayType, typename DataType>
void permutation_helper(ArrayType* a, void (*callback)(ArrayType*, DataType), size_t p, DataType opaque_data) {
  size_t i;
  // When we reach the end, we're done
  if (p+1 >= a->len()) {
    callback(a, opaque_data);
    return;
  }
  // No permutation, just call
  permutation_helper(a, callback, p+1, opaque_data);
  // Then swap with each other option, and call
  for (i=p+1; i<a->len(); i++) {
    a->swap(p,i);
    permutation_helper(a, callback, p+1, opaque_data);
    a->swap(p,i);
  }
}

template<typename ArrayType, typename DataType>
void permutations(Array<uint32_t>* a, void (*callback)(ArrayType*, void*), DataType opaque_data) {
  permutation_helper<ArrayType>(a, callback, 0, opaque_data);
}

int main(){
  printf("Begin Sort.h unittest\n");
  Array<uint32_t> testdata; 
  // Exhaustive testing of up to 8 elements
  for (int x=0; x<10; x++) {
    testdata.resize(x);
    if (x>0) {
      testdata[x-1] = x;
    }
    permutations<Array<uint32_t>, void*>(&testdata, run, NULL); 
  }
  // Random testing of large arrays
  for (uint32_t i=1000; i<1500; i+=1) {
    testdata.resize(i);
    for (uint32_t x=0; x<i; x++) {
      testdata[x] = rand();
    }
    run(&testdata, NULL);
  }
  printf("PASS\n");
  return 0;
}
