#include <stdio.h>
#include "set.h"

int main(int argc, char *argv[]) {
  printf("Begin Set.h unittest\n");
  Set<int> s;
  int i;
  for (i=0; i<100; i++) {
    s.insert(i);
    s.get(i);
  }
  for (i=0; i<100; i++) {
    s.remove(i);
  }
  printf("PASS\n");
}

