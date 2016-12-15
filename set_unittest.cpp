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

  printf("PASS\n");
}

