#include <stdio.h>
#include <time.h>
#include "panic.h"
#include "dict.h"

// This is so we can script sets of tests at different arities
#ifndef TEST_SIZE
#define TEST_SIZE 20000000
#endif

int ints[TEST_SIZE];
int ints_start;
int ints_end;

int main(int argc, char* argv[]) {
  Dict<int, int> tree;

  int i;
  int gets=0;
  printf("Begin Dict.h benchmark\n");
  ints_end=0;
  ints_start=0;
  for (i=0; i<TEST_SIZE; i++) {
    bool new_v = false;
    int r;
    // find a value we haven't used yet
    while (!new_v) {
      // Note, we did not initialize rand, this is purposeful
      r = rand();
      new_v = !tree.get(r); 
      gets++;
    }
    // put it in thet tree
    tree.insert(r, r);
    // and in the list
    ints[ints_end++] = r;
  }
  for(i=0; i<ints_end; i++) {
    int junk;
    tree.remove(ints[i], &junk);
    ints_start += 1;
  }
  printf("gets %d\n", gets);
}

