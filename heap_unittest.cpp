#define HEAP_DEBUG
#define ARRAY_DEBUG
#include "heap.h"

#define TEST_SIZE 200

class HeapCompare {
  public:
    static int compare(int *v1, int *v2) {
      return (*v1) - (*v2);
    }
};

int main(int argc, char **argv) {
  printf("Begin Heap.h/Array.h unittest\n");
  Heap<int, HeapCompare> heap;
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
