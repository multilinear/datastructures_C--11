/* Copywrite Matthew Brewer 2017-03-22
 *
 * This is a benchmark for dictionaries using internal allocation
 * We decide WHAT we're testing using the macro system
 * Our Makefile takes advantage of this.
 *
 * This seems odd, but it avoids code duplication and ensures all our
 * datastructures have the same API (Except where it really isn't a good idea)
 */

#include <stdio.h>
#include <time.h>
#include "panic.h"

#define TEST_ITERATIONS 1000000
#define TEST_SIZE 100

#ifdef TEST_BTREE
// This is so we can script sets of tests at different arities
#include "btree.h"
#ifndef ARITY
#define ARITY 64 
#endif
#endif

#ifdef TEST_TS_BTREE
// This is so we can script sets of tests at different arities
#include "ts_btree.h"
#ifndef ARITY
#define ARITY 64 
#endif
#endif

#ifdef TEST_BTREEHASHTABLE
#include "btreehashtable.h"
#endif

class Comp {
  // For use with T=int, Val_T=int
  public:
    static const int val(const int T) {
      return T;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    static size_t hash(int v) {
      return v;
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
  #ifdef TEST_BTREE
  printf("Begin BTree.h benchmark\n");
  BTree<int, int, Comp, ARITY> dict;
  #endif
  #ifdef TEST_TS_BTREE
  printf("Begin BTree.h benchmark\n");
  TSBTree<int, int, Comp, ARITY> dict;
  #endif
  #ifdef TEST_BTREEHASHTABLE
  printf("Begin BTreeHashTable.h benchmark\n");
  BTreeHashTable<int, int, Comp> dict; 
  #endif

  int j;
  int get_count=0;
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
        #ifdef TEST_TS_BTREE
        int tmp;
        new_v = !dict.get(r, &tmp); 
        #else
        new_v = !dict.get(r); 
        #endif
        get_count++;
      }
      // put it in the dict
      dict.insert(r);
      // and in the list
      ints[ints_end++] = r;
    }
    for(i=0; i<ints_end; i++) {
      int junk;
      dict.remove(ints[i], &junk);
    }
  }
  printf("get_count %d\n", get_count);
}

