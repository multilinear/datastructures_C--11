#include <stdio.h>
#include "dict.h"

int main(int argc, char *argv[]) {
  printf("Begin Dict.h unittest\n");
  Dict<int, int> d;
  int i;
  for (i=0; i<100; i++) {
    d.insert(i, 999);
    d.set(i, 998);
    d.get(i);
  }
  for (i=0; i<100; i++) {
    int ret;
    d.remove(i, &ret);
    if (ret != 998) {
      PANIC("Set is failing\n");
    }
  }
  for (i=0; i<100; i++) {
    d.set(i, 999);
    d.get(i);
  }
  for (i=0; i<100; i++) {
    int ret;
    d.remove(i, &ret);
    if (ret != 999) {
      PANIC("Set is failing\n");
    }
  }

  printf("PASS\n");
}

