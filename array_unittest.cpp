#define ARRAY_DEBUG

#include <stdio.h>

#ifdef TEST_ARRAY
#include "array.h"
#define RESIZABLE
#endif

#ifdef TEST_UARRAY
#include "array.h"
#define RESIZABLE
#define UARRAY 
#endif

#ifdef TEST_STATICARRAY
#include "array.h"
#endif

#ifdef TEST_STATICUARRAY
#include "array.h"
#define RESIZABLE
#define UARRAY 
#endif

#ifdef TEST_DCUARRAY
#include "delayed_copy_array.h"
#define RESIZABLE
#define UARRAY 
#endif

#ifdef TEST_DICTARRAY
#include "dictarray.h"
//#define RESIZABLE (tests fail, 'cause we don't REALLY resize)
#define UARRAY 
#endif

#ifdef TEST_TREEARRAY
#include "treearray.h"
#define RESIZABLE
#endif
#ifdef TEST_TREEUARRAY
#include "treearray.h"
#define RESIZABLE
#define USED
#endif



#ifdef TEST_ZEROARRAY
#include "zero_array.h"
#endif



template<typename AT>
void base_test(void) {
  int test_data[] = {1,2,3,4};
  // Testing array initialization
  AT a(test_data, 4);
  if (a.size() != 4) {
    printf("a.size() = %lu\n", a.size());
    PANIC("Size is wrong");
  }
  for (int i=0;i<4;i++) {
    if (test_data[i] != a[i]) {
      printf("index %d is %d not %d\n", i,  a[i], test_data[i]);
      PANIC("data is wrong");
    }
  }
  // Testing swap
  a.swap(0,1);
  if (a[0] != test_data[1] || a[1] != test_data[0]) {
    PANIC("Swap doesn't work");
  }
  // Testing revi
  if (a.revi(1) != a[a.size()-1]) {
    PANIC("Revi doesn't work");
  }
  if (a.revi(2) != a[a.size()-2]) {
    PANIC("Revi doesn't work");
  }
}

template<typename AT>
void resizable_test(void) {
  int test_data[] = {1,2,3,4};
  // Testing array initialization
  AT a(test_data, 4);
  // Testing resize (to smaller only)
  a.resize(3);
  if (a.size() != 3) {
    printf("a.size() = %lu, not 3\n", a.size());
    PANIC("Resize failed");
  }
  if (a.revi(1) != 3) {
    printf("a.revi(1)=%d\n", a.revi(1));
    PANIC("Resize revi semantics are wrong");
  }
  if (a.isempty()) {
    PANIC("Array is empty when it has data!");
  }
  if (!a) {
    PANIC("Array is empty when it has data!");
  }

}

template<typename AT>
void used_test(void) {
  int test_data[] = {1};
  AT ua(test_data, 1);
  // push
  ua.push(6);
  if (ua.size() != 2 && ua.revi(1) != 6) {
    PANIC("UA push is broken");
  }
  // pop
  int res=0;
  bool b;
  b = ua.pop(&res);
  if (b != true && res != 6) {
    PANIC("UA pop is broken");
  }
  ua.pop(&res);
  b = ua.pop(&res);
  if (b != false) {
    PANIC("UA pop underflow is broken got");
  }
  // drop
  ua.push(1);
  ua.drop();
  if (ua.size() != 0) {
    PANIC("UA drop is broken");
  }
  ua.drop();
  if (ua.size() != 0) {
    PANIC("UA drop underflow is broken");
  }
  // full
  if (ua.isfull()) {
    PANIC("UA is full when it's empty!");
  }
  ua.push(1);
  if (ua.isfull()) {
    PANIC("UA is full with only one element!");
  }
  if (!ua) {
    PANIC("bool conversion doesn't work");
  }
}

int main(void) {
  #ifdef TEST_STATICARRAY
  printf("Begin StaticArray unittest\n");
  StaticArray<int,4> a;
  #endif
  #ifdef TEST_STATICUARRAY
  printf("Begin StaticUArray unittest\n");
  StaticUArray<int,4> a;
  #endif
  #ifdef TEST_ARRAY 
  printf("Begin Array unittest\n");
  Array<int> a;
  #endif
  #ifdef TEST_UARRAY
  printf("Begin UArray unittest\n");
  UArray<int> a;
  #endif
  #ifdef TEST_DCUARRAY
  printf("Begin DCUArray unittest\n");
  DCUArray<int> a;
  #endif
  #ifdef TEST_DICTARRAY
  printf("Begin DictArray unittest\n");
  DictUArray<int> a;
  #endif
  #ifdef TEST_TREEARRAY
  printf("Begin TreeArray unittest\n");
  TreeArray<int,4> a;
  #endif
  #ifdef TEST_TREEUARRAY
  printf("Begin TreeUArray unittest\n");
  TreeUArray<int,4> a;
  #endif
  #ifdef TEST_ZEROARRAY
  printf("Begin ZeroArray unittest\n");
  ZeroArray<int> a;
  #endif

  base_test<decltype(a)>(); 
  #ifdef RESIZABLE
  resizable_test<decltype(a)>();
  #endif
  #ifdef USED
  used_test<decltype(a)>();
  #endif

  // ***** Now for extra tests specific to a given structure
  #ifdef TEST_UARRAY
  int test_data[] = {1};
  // Used Array specific tests
  UArray<int> ua(test_data, 1);
  if (ua.isfull()) {
    PANIC("UArray should not be full");
  }
  ua.resize(10);
  if (ua.size() != 10) {
    PANIC("Used array resize up failed");
  }
  #endif
  #ifdef TEST_STATICUARRAY
  int test_data[] = {1};
  StaticUArray<int,1> sa(test_data, 1);
  if (!sa.isfull()) {
    PANIC("StaticUArray should be full");
  }
  #endif
  #ifdef TEST_TREEARRAY
  TreeArray<int,2> at(10);
  at.resize(100);
  for (int i=0;i<100;i++) {
    at[i] = i;
  }
  for (int i=0;i<100;i++) {
    if (at[i] != i) {
      PANIC("ArrayTree is corrupt");
    }
  }
  at.resize(20);
  for (int i=0;i<20;i++) {
    if (at[i] != i) {
      PANIC("ArrayTree is corrupt");
    }
  }
  at.resize(10000);
  for (int i=0;i<10000;i++) {
    at[i] = i;
  }
  for (int i=0;i<10000;i++) {
    if (at[i] != i) {
      PANIC("ArrayTree is corrupt");
    }
  }
  #endif

  printf("PASS\n");
  return 0;
}
