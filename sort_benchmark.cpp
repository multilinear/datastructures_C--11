
#include "stdint.h"
#include "sort.h"
#include "array.h"

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS 1000000
#endif
#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif

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
  for (uint32_t j = 0; j<TEST_ITERATIONS; j++) {
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

