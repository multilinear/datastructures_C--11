#define ARRAY_DEBUG

#include <cstdint>
#include <stdio.h>
#include "panic.h"
#include "sort.h"
#include "permutations.h"
#include <vector>

class IntCompare {
  public:
    static int32_t compare(uint32_t v1, uint32_t v2) {
      return v1 - v2;
    }
};

void print_array(std::vector<uint32_t> *a) {
  printf("[");
  for (size_t i=0; i<a->size(); i++) {
    printf("%d,", (*a)[i]);
  }
  printf("]\n");
}

void run(std::vector<uint32_t>* input, int _unused){
  std::vector<uint32_t> *bad = nullptr;
  //printf("Run\n");
  std::vector<uint32_t> s_a(*input);  
  std::vector<uint32_t> b_a(*input);  
  std::vector<uint32_t> q_a(*input);  
  std::vector<uint32_t> m_a(*input);  
  std::vector<uint32_t> h_a(*input);  
  std::vector<uint32_t> br_a(*input);  
  std::vector<uint32_t> r_a(*input);  
  std::vector<uint32_t> fast_a(*input);  
  std::vector<uint32_t> tmp_a(m_a.size());  
  //printf("testing: ");
  //print_array(input);
  //printf("Selection: ");
  selection_sort<std::vector<uint32_t>,IntCompare>(&s_a);
  //print_array(&s_a);
  //printf("Bubble: ");
  bubble_sort<std::vector<uint32_t>,IntCompare>(&b_a);
  //print_array(&q_a);
  //printf("Quick: ");
  quick_sort<std::vector<uint32_t>,IntCompare>(&q_a);
  //printf("Merge Test %lu\n", m_a.size());
  //print_array(&m_a);
  //printf("Merge: ");
  merge_sort<std::vector<uint32_t>,std::vector<uint32_t>,IntCompare>(&m_a, &tmp_a);
  //print_array(&m_a);
  heap_sort<std::vector<uint32_t>,IntCompare>(&h_a);
  //printf("bradix_sort input\n");
  //print_array(&br_a);
  bradix_sort<std::vector<uint32_t>>(&br_a);
  //printf("bradix_sort output\n");
  radix_sort<std::vector<uint32_t>,std::vector<uint32_t>, 5>(&r_a, &tmp_a);
  fast_sort<std::vector<uint32_t>>(&fast_a, &tmp_a);
  for (size_t i=0; i<s_a.size(); i++) {
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
    if (s_a[i] != fast_a[i]) {
      bad = &fast_a;
    }
  }
  if (bad) {
    printf("ERROR! failed to sort\n");
    printf("Length was %li\n", input->size());
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


int main(){
  printf("Begin Sort.h unittest\n");
  std::vector<uint32_t> testdata; 
  // Exhaustive testing of up to 8 elements
  for (int x=0; x<10; x++) {
    testdata.resize(x);
    if (x>0) {
      testdata[x-1] = x;
    }
    permutations<std::vector<uint32_t>, int>(&testdata, run, 0); 
  }
  // Random testing of large arrays
  for (uint32_t i=1000; i<1500; i+=1) {
    testdata.resize(i);
    for (uint32_t x=0; x<i; x++) {
      testdata[x] = rand();
    }
    run(&testdata, 0);
  }
  printf("PASS\n");
  return 0;
}
