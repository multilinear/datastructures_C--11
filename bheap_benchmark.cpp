#include "bheap.h"

#ifndef NODE_SIZE
#define NODE_SIZE 3
#endif
#define TEST_SIZE 10000000

class BHeapCompare {
  public:
    static int val(int v) {
      return v;
    }
    static int compare(int *v1, int *v2) {
      return *v1 - *v2;
    }
};

int main(int argc, char **argv) {
  printf("Begin BHeap.h/Array.h benchmark\n");
  BHeap<int, int, BHeapCompare, NODE_SIZE> heap;
  int i,j;
  int val=-1;

  for (j=0;j<TEST_SIZE;j++) {
    heap.push(rand());
  }
  for (j=0;j<TEST_SIZE;j++) {
    heap.pop(&val);
  }

  printf("PASS\n");
  return 0;
}
