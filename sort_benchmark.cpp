
#include "stdint.h"
#include "sort.h"
#include "timer.h"
#include <vector>

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS 1000000
#endif
#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif
#ifndef RADIX_BITS
#define RADIX_BITS 6
#endif

class IntComparitor {
  public:
    static int32_t compare(uint32_t v1, uint32_t v2) {
      return v1 - v2;
    }
};

int main(int argc, char **argv) {
  #ifdef TEST_SELECTSORT
  printf("SelectSort ");
  #endif
  #ifdef TEST_BUBBLESORT
  printf("BubbleSort ");
  #endif
  #ifdef TEST_QUICKSORT
  printf("QuickSort ");
  #endif
  #ifdef TEST_HEAPSORT
  printf("HeapSort ");
  #endif
  #ifdef TEST_MERGESORT
  printf("MergeSort ");
  std::vector<uint32_t> b(TEST_SIZE);
  #endif
  #ifdef TEST_BRADIXSORT
  printf("BRadixSort ");
  #endif
  #ifdef TEST_RADIXSORT
  printf("RadixSort ");
  std::vector<uint32_t> b(TEST_SIZE);
  #endif
  #ifdef TEST_FASTSORT
  printf("FastSort ");
  std::vector<uint32_t> b(TEST_SIZE);
  #endif
  std::vector<uint32_t> a(TEST_SIZE);
  timeb t1, t2;
  ftime(&t1);
  for (uint32_t j = 0; j<TEST_ITERATIONS; j++) {
    for (uint32_t i = 0; i<TEST_SIZE; i++) {
      a[i] = rand();
    }
    #ifdef TEST_SELECTSORT
    selection_sort<std::vector<uint32_t>, IntComparitor>(&a);
    #endif
    #ifdef TEST_BUBBLESORT
    bubble_sort<std::vector<uint32_t>, IntComparitor>(&a);
    #endif
    #ifdef TEST_QUICKSORT
    quick_sort<std::vector<uint32_t>, IntComparitor>(&a);
    #endif
    #ifdef TEST_HEAPSORT
    heap_sort<std::vector<uint32_t>, IntComparitor>(&a);
    #endif
    #ifdef TEST_MERGESORT
    merge_sort<std::vector<uint32_t>,std::vector<uint32_t>, IntComparitor>(&a, &b);
    #endif
    #ifdef TEST_BRADIXSORT
    bradix_sort<std::vector<uint32_t>>(&a);
    #endif
    #ifdef TEST_RADIXSORT
    radix_sort<std::vector<uint32_t>,std::vector<uint32_t>, RADIX_BITS>(&a, &b);
    #endif
    #ifdef TEST_FASTSORT
    fast_sort<std::vector<uint32_t>, std::vector<uint32_t>>(&a, &b);
    #endif
  }
  ftime(&t2);
  double t = tdiff(t2,t1);
	// Verify
	for (size_t i = 0; i<a.size()-1; i++) {
		if (a[i] > a[i+1]) {
      printf("%u > %u\n", a[i], a[i+1]);
			PANIC("Sort isn't sorting!");
		}
	}
  printf("test_size=%i test_iterations=%i time=%lf radix_bits=%u\n", TEST_SIZE, TEST_ITERATIONS, t, RADIX_BITS);
  return 0;
}

