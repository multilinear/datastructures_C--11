#include <stdio.h>
#include "set.h"

int main(int argc, char *argv[]) {
  printf("Begin Set.h unittest\n");
  Set<int> s;
  int i;
  // Check basic inserting and removing
  for (i=0; i<100; i++) {
    if(!s.insert(i)) {
      PANIC("Insert doesn't work");
    }
  }
  for (i=0; i<100; i++) {
    if (!s.contains(i)) {
      PANIC("Get doesn't work");
    }
    if (s.contains(101)) {
      PANIC("Get is finding stuff not there");
    }
  }
  if (s.isempty()) {
    PANIC("List is not empty, reports empty");
  }
  for (i=0; i<100; i++) {
    if(!s.remove(i)) {
      PANIC("Remove doesn't work");
    }
  }
  for (i=0; i<100; i++) {
    if(s.contains(i)) {
      PANIC("Remove doesn't work");
    }
  }
  if (!s.isempty()) {
    PANIC("List is empty, reports not empty");
  }

  // Test iterator
  for (size_t i=0; i<100; i++) {
    s.insert(i);
  }
  i = 0;
  for (auto i2 = s.begin(); i2 != s.end(); ++i2) {
    if (*i2 != i) {
      PANIC("Set iterator broken");
    }
    ++i;
  }

  // Test set operations
  Set<int> s1;
  for (int i=0; i<10; i++) {
    s1.insert(i);
  }
  Set<int> s2;
  for (int i=5; i<15; i++) {
    s2.insert(i);
  }
  Set<int> s3;
  for (int i=0; i<5; i++) {
    s3.insert(i);
  }
  Set<int> s4;
  for (int i=10; i<15; i++) {
    s4.insert(i);
  }
  Set<int> s5;
  for (int i=5; i<10; i++) {
    s5.insert(i);
  }

  if (s3 != (s1-s2)) {
    PANIC("Subtraction doesn't work");
  }
  if (s2+s3 != (s1+s4)) {
    PANIC("Addition doesn't work");
  }
  if (s5 != (s1&s2)) {
    PANIC("Intersection doesn't work");
  }
  if (s2+s3 != (s1|s4)) {
    PANIC("Union doesn't work");
  }
  if (s3+s5 != (s1^s4)) {
    PANIC("co-intersection doesn't work");
  }

  printf("PASS\n");
}

