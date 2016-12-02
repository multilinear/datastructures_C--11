#include "heap.h"
#include "array.h"
#include "arraytree.h"

#define TEST_SIZE 10000000

class HeapCompare {
  public:
    static int val(int v) {
      return v;
    }
    static int compare(int *v1, int *v2) {
      return *v1 - *v2;
    }
};

int main(int argc, char **argv) {
  printf("Begin Heap.h/Array.h benchmark\n");
  Heap<UArrayTree<int, 12>, int, HeapCompare> heap;
  //Heap<UArray<int>, int, HeapCompare> heap;
  int j;
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
