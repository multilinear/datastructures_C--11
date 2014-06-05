#define HEAP_DEBUG
#include "heap.h"

#define TEST_SIZE 200

class HeapCompare {
  public:
    static int val(int v) {
      return v;
    }
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
};

int main(int argc, char **argv) {
  printf("Begin Heap.h/Array.h unittest\n");
  Heap<int, int, HeapCompare> heap;
  int i,j;
  for (j=0;j<TEST_SIZE;j++) {
    for (i=0;i<j;i++) {
      heap.push(i);
    }
    for (i=0;i<j;i++) {
      heap.pop();
    }
    for (i=j;i>0;i--) {
      heap.push(i);
    }
    for (i=j;i>0;i--) {
      heap.pop();
    }
  }
  for (j=0;j<TEST_SIZE;j++) {
    for (i=0;i<j;i++) {
      heap.push(rand());
    }
    for (i=0;i<j;i++) {
      heap.pop();
    }
  }

  printf("PASS\n");
}
