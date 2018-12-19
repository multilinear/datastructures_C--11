#include "stdint.h"
#include "sort.h"
#include "medianfind.h"
#include "array.h"
#include "timer.h"

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS 1000000
#endif
#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif

class IntComparitor {
  public:
    static int32_t compare(uint32_t v1, uint32_t v2) {
      return v1 - v2;
    }
};

int main(int argc, char **argv) {
  #ifdef TEST_SORTSELECT
  printf("Sort ");
  #endif
  #ifdef TEST_QUICKSELECT
  printf("QuickSelect ");
  #endif
  #ifdef TEST_LINEARQUICKSELECT
  printf("LinearQuickSelect ");
  #endif
  Array<uint32_t> a(TEST_SIZE);
  timeb t1, t2;
  ftime(&t1);
  for (uint32_t j = 0; j<TEST_ITERATIONS; j++) {
    for (uint32_t i = 2; i<TEST_SIZE; i++) {
      a[i] = rand();
    }
    #ifdef TEST_SORTSELECT
    quick_sort<Array<uint32_t>, IntComparitor>(&a);
    #endif
    #ifdef TEST_QUICKSELECT
    findkth<Array<uint32_t>, IntComparitor, false>(&a, (a.len()-1)/2);
    #endif
    #ifdef TEST_LINEARQUICKSELECT
    findkth<Array<uint32_t>, IntComparitor, true>(&a, (a.len()-1)/2);
    #endif
  }
  ftime(&t2);
  double t = tdiff(t2,t1);
  printf("test_size=%i test_iterations=%i time=%lf\n", TEST_SIZE, TEST_ITERATIONS, t);
  return 0;
}

