#define ARRAY_DEBUG

#include <stdio.h>
#include "panic.h"
#include "array.h"
#include "sort.h"

class IntCompare {
  public:
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
};

void print_array(Array<int> *a) {
  printf("[");
  for (size_t i=0; i<a->len(); i++) {
    printf("%d,", (*a)[i]);
  }
  printf("]\n");
}

void run(Array<int>* input, void* _unused){
  bool bad = false;
  //printf("Run\n");
  Array<int> s_a(input);  
  Array<int> b_a(input);  
  Array<int> q_a(input);  
  Array<int> m_a(input);  
  Array<int> tmp_a(m_a.len());  
  //printf("testing: ");
  //print_array(input);
  //printf("Selection: ");
  selection_sort<Array<int>,IntCompare>(&s_a);
  //print_array(&s_a);
  //printf("Bubble: ");
  bubble_sort<Array<int>,IntCompare>(&b_a);
  //print_array(&q_a);
  //printf("Quick: ");
  quick_sort<Array<int>,IntCompare>(&q_a);
  //printf("Merge Test %lu\n", m_a.len());
  //print_array(&m_a);
  //printf("Merge: ");
  merge_sort<Array<int>,Array<int>,IntCompare>(&m_a, &tmp_a);
  //print_array(&m_a);
  for (size_t i=0; i<s_a.len(); i++) {
    if (s_a[i] != b_a[i]) {
      bad = true;
    }
    if (s_a[i] != q_a[i]) {
      bad = true;
    }
    if (s_a[i] != m_a[i]) {
      bad = true;
    }
  }
  if (bad) {
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
void permutations(Array<int>* a, void (*callback)(ArrayType*, void*), DataType opaque_data) {
  permutation_helper<ArrayType>(a, callback, 0, opaque_data);
}

int main(){
  printf("Begin Sort.h unittest\n");
  Array<int> testdata; 
  // Exhaustive testing of up to 9 elements
  for (int x=0; x<10; x++) {
    testdata.resize(x);
    if (x>0) {
      testdata[x-1] = x;
    }
    permutations<Array<int>, void*>(&testdata, run, NULL); 
  }
  // Random testing of large arrays
  for (int i=1000; i<1500; i+=1) {
    testdata.resize(i);
    for (int x=0; x<i; x++) {
      testdata[x] = rand();
    }
    run(&testdata, NULL);
  }
  printf("PASS\n");
  return 0;
}
