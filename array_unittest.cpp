#define ARRAY_DEBUG

#include <stdio.h>
#include "array.h"

template<typename AT>
void base_test(void) {
  int test_data[] = {1,2,3,4};
  // Testing array initialization
  AT a(test_data, 4);
  if (a.len() != 4) {
    PANIC("Size is wrong");
  }
  for (i=0;i<4;i++) {
    if (test_data[i] != a[i]) {
      PANIC("data is wrong");
    }
  }
  // Testing swap
  a.swap(0,1);
  if (a[0] != test_data[1] || a[1] != test_data[0]) {
    PANIC("Swap doesn't work");
  }
  // Testing revi
  if (a.revi(0) != a[a.len()-1] || a.revi(1) != a[a.len()-2]) {
    PANIC("Revi doesn't work");
  }
}

int main(void) {
  printf("Array unittest");
  base_test<Array<int>>();
  base_test<UsedArray<int>>();
  base_test<StaticArray<int,4>>();
  printf("PASS");
  return 0;
}
