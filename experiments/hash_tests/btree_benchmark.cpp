#include <stdio.h>
#include <time.h>
#include "panic.h"
#include "btree.h"

// This is so we can script sets of tests at different arities
#ifndef ARITY
#define ARITY 64 
#endif
#define TEST_ITERATIONS 1000000
#define TEST_SIZE 100

class BTreeComp {
  // For use with T=int, Val_T=int
  public:
    static const int val(const int T) {
      return T;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    static void printT(const int t) {
      printf("%d", t);
    }
    static void printV(const int v) {
      printf("%d", v);
    }
};


int ints[TEST_SIZE];
int ints_end;

int main(int argc, char* argv[]) {
  BTree<int, int, BTreeComp, ARITY> tree;

  int j;
  int get_count=0;
  printf("Begin BTree.h benchmark\n");
  for (j=0; j<TEST_ITERATIONS; j++) {
    int i;
    ints_end=0;
    for (i=0; i<TEST_SIZE; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = !tree.get(r); 
        get_count++;
      }
      // put it in thet tree
      tree.insert(r);
      // and in the list
      ints[ints_end++] = r;
    }
    for(i=0; i<ints_end; i++) {
      int junk;
      tree.remove(ints[i], &junk);
    }
  }
  printf("get_count %d\n", get_count);
}

