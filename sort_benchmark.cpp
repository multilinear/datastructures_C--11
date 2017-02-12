
#include "stdint.h"
#include "sort.h"
#include "array.h"

#define TEST_SIZE 10000000 
#define PASSES 100

class IntComparitor {
  public:
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
};

int main(int argc, char **argv) {
  printf("Begin sort benchmark\n");
  Array<int> a(TEST_SIZE);
  //Array<int> b(TEST_SIZE);
  for (uint32_t j = 0; j<PASSES; j++) {
    for (uint32_t i = 0; i<TEST_SIZE; i++) {
      a[i] = rand();
    }
    //quick_sort<Array<int>, IntComparitor>(&a);
    heap_sort<Array<int>, IntComparitor>(&a);
    //merge_sort<Array<int>,Array<int>, IntComparitor>(&a, &b);
  }
  printf("Done\n");
  return 0;
}

