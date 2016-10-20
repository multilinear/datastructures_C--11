#include <stdio.h>
#include "set.h"

int main(int argc, char *argv[]) {
  printf("Begin Set.h unittest\n");
  Set<int> s;
  int i;
  // Check basic adding and removing
  for (i=0; i<100; i++) {
    if(!s.add(i)) {
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

  printf("PASS\n");
}

