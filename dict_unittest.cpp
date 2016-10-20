#include <stdio.h>
#include "dict.h"

int main(int argc, char *argv[]) {
  printf("Begin Dict.h unittest\n");
  Dict<int, int> d;
  int i;
  int ret;
  // Test insert normal 
  for (i=0; i<100; i++) {
    d.insert(i, 998);
    d.get(i);
  }
  for (i=0; i<100; i++) {
    ret = -1;
    d.remove(i, &ret);
    if (ret != 998) {
      PANIC("Set is failing\n");
    }
  }
  if (!d.isempty()) {
    PANIC("Not empty");
  }

  // Test insert duplicate
  if (!d.insert(1, 999)) {
    PANIC("Failure to insert?");
  }
  if (d.insert(1, 998)) {
    PANIC("Succeeded at inserting same key twice");
  }
  if (d.isempty()) {
    PANIC("showing empty while not empty");
  }
  if (!d.remove(1, &ret)) {
    PANIC("Failure to remove after duplicate insert");
  }
  if (ret != 999) {
    PANIC("insert acted as override");
  }
  if (d.remove(1, &ret)) {
    PANIC("succeeded in removing element twice");
  }
  if (!d.isempty()) {
    PANIC("Not empty");
  }

  // Test set normal 
  for (i=0; i<100; i++) {
    d.set(i, 998);
    d.get(i);
  }
  for (i=0; i<100; i++) {
    int ret = -1;
    d.remove(i, &ret);
    if (ret != 998) {
      PANIC("Set is failing\n");
    }
  }
  // Test set Override
  for (i=0; i<100; i++) {
    d.insert(i, 999);
    d.set(i, 998);
    d.get(i);
  }
  for (i=0; i<100; i++) {
    int ret = -1;
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
    int ret = -1;
    d.remove(i, &ret);
    if (ret != 999) {
      PANIC("Set is failing\n");
    }
  }

  printf("PASS\n");
}

