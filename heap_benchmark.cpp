#include "heap.h"

#define TEST_SIZE 100000000

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
    heap.push(rand());
  }
  for (j=0;j<TEST_SIZE;j++) {
    heap.pop();
  }

  printf("PASS\n");
}