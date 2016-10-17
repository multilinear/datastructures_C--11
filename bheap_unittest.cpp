//#define BHEAP_DEBUG
#define ARRAY_DEBUG
#define BHEAP_DEBUG
//#define BHEAP_DEBUG_VERBOSE
#include "bheap.h"

// This needs a large test size, since we special case
// some pretty large cases
#define TEST_SIZE 1000 
#ifndef NODE_SIZE
#define NODE_SIZE 4 
#endif

class BHeapCompare {
  public:
    static int compare(int *v1, int *v2) {
      return *v1 - *v2;
    }
};

int main(int argc, char **argv) {
  printf("Begin BHeap.h/Array.h unittest\n");
  BHeap<int, BHeapCompare, NODE_SIZE> heap;
  int i,j;
  int val=-1;
  for (j=TEST_SIZE-1;j<TEST_SIZE;j++) {
    j=TEST_SIZE;
    for (i=0;i<j;i++) {
      heap.push(i);
    }
    for (i=0;i<j;i++) {
      heap.pop(&val);
    }
    if (heap.pop(&val)) {
      PANIC("Bheap didn't drain");
    }
    for (i=j;i>0;i--) {
      heap.push(i);
    }
    for (i=j;i>0;i--) {
      heap.pop(&val);
    }
    if (heap.pop(&val)) {
      PANIC("Bheap didn't drain");
    }
  }
  for (j=0;j<TEST_SIZE;j++) {
    for (i=0;i<j;i++) {
      val = rand();
      heap.push(val);
    }
    for (i=0;i<j;i++) {
      heap.pop(&val);
    }
    if (heap.pop(&val)) {
      PANIC("Bheap didn't drain");
    }
  }

  printf("PASS\n");
  return 0;
}
