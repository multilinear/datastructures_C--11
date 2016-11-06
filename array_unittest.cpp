#define ARRAY_DEBUG

#include <stdio.h>
#include "array.h"
#include "arraytree.h"

template<typename AT>
void base_test(void) {
  int test_data[] = {1,2,3,4};
  // Testing array initialization
  AT a(test_data, 4);
  if (a.len() != 4) {
    printf("a.len() = %lu\n", a.len());
    PANIC("Size is wrong");
  }
  for (int i=0;i<4;i++) {
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
  if (a.revi(1) != a[a.len()-1]) {
    PANIC("Revi doesn't work");
  }
  if (a.revi(2) != a[a.len()-2]) {
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
  if (a.len() != 3) {
    PANIC("Resize failed");
  }
  if (a.revi(1) != 3) {
    PANIC("Resize revi semantics are wrong");
  }
}

template<typename AT>
void used_test(void) {
  int test_data[] = {1};
  AT ua(test_data, 1);
  // push
  ua.push(6);
  if (ua.len() != 2 && ua.revi(1) != 6) {
    PANIC("UA push is broken");
  }
  // pop
  int res;
  bool b;
  b = ua.pop(&res);
  if (b != true && res != 6) {
    PANIC("UA pop is broken");
  }
  ua.pop(&res);
  b = ua.pop(&res);
  if (b != false) {
    PANIC("UA pop underflow is broken");
  }
  // drop
  ua.push(1);
  ua.drop();
  if (ua.len() != 0) {
    PANIC("UA drop is broken");
  }
  ua.drop();
  if (ua.len() != 0) {
    PANIC("UA drop underflow is broken");
  }
  // full
  if (ua.is_full()) {
    PANIC("UA is full when it's empty!");
  }
  ua.push(1);
  if (ua.is_full()) {
    PANIC("UA is full with only one element!");
  }
}

int main(void) {
  printf("Begin Array.h ArrayTree.h unittest\n");
  base_test<StaticArray<int,4>>();
  base_test<StaticUArray<int,4>>();
  base_test<Array<int>>();
  base_test<UArray<int>>();
  base_test<ArrayTree<int,1>>();
  resizable_test<StaticUArray<int,4>>();
  resizable_test<Array<int>>();
  resizable_test<UArray<int>>();
  resizable_test<ArrayTree<int,1>>();
  used_test<StaticUArray<int,4>>();
  used_test<UArray<int>>();
  int test_data[] = {1};
  // Static used array specific tests
  StaticUArray<int,1> sa(test_data, 1);
  if (!sa.is_full()) {
    PANIC("StaticUArray should be full");
  }
  // Used Array specific tests
  UArray<int> ua(test_data, 1);
  if (ua.is_full()) {
    PANIC("UArray should not be full");
  }
  ua.resize(10);
  if (ua.len() != 10) {
    PANIC("Used array resize up failed");
  }
  // ArrayTree specific tests
  ArrayTree<int,2> at(10);
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
  printf("PASS\n");
  return 0;
}
