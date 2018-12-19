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
#include <stdint.h>
#include "panic.h"
#include "timer.h"

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS 1000000
#endif
#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif
#ifndef ARITY
#define ARITY 64 
#endif

#ifdef TEST_BTREE
// This is so we can script sets of tests at different arities
#include "btree.h"
#endif

#ifdef TEST_TS_BTREE
// This is so we can script sets of tests at different arities
#include "ts_btree.h"
#endif

#ifdef TEST_BTREEHASHTABLE
#include "btreehashtable.h"
#endif

#ifdef TEST_HASHTABLE
#include "hashtable.h"
#endif

class Comp {
  // For use with T=int, Val_T=int
  public:
    static const uint64_t val(const uint64_t T) {
      return T;
    }
    static const int compare(const uint64_t v1, const uint64_t v2) {
      if (v1 > v2) return 1;
      if (v1 < v2) return -1;
      return 0;
      //return v1-v2;
    }
    static size_t hash(uint64_t v) {
      return v;
    }
    static void printT(const uint64_t t) {
      printf("%ld", t);
    }
    static void printV(const uint64_t v) {
      printf("%ld", v);
    }
};


uint64_t ints[TEST_SIZE];
uint64_t ints_end;

int main(int argc, char* argv[]) {
  #ifdef TEST_BTREE
  printf("BTree.h ");
  BTree<uint64_t, uint64_t, Comp, ARITY> dict;
  #endif
  #ifdef TEST_TS_BTREE
  printf("TS_BTree.h ");
  TSBTree<uint64_t, uint64_t, Comp, ARITY> dict;
  #endif
  #ifdef TEST_BTREEHASHTABLE
  printf("BTreeHashTable.h ");
  BTreeHashTable<uint64_t, uint64_t, Comp> dict; 
  #endif
  #ifdef TEST_HASHTABLE
  printf("HashTable.h ");
  HashTable<uint64_t, uint64_t, Comp> dict; 
  #endif

  timeb t1, t2;
  ftime(&t1);
  uint64_t j;
  uint64_t get_count=0;
  for (j=0; j<TEST_ITERATIONS; j++) {
    uint64_t i;
    ints_end=0;
    for (i=0; i<TEST_SIZE; i++) {
      bool new_v = false;
      uint64_t r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        // Assumes RAND_MAX=INT_MAX and int=32 bits
        r = rand()*rand();
        #ifdef TEST_TS_BTREE
        uint64_t tmp;
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
      uint64_t junk;
      dict.remove(ints[i], &junk);
    }
  }
  ftime(&t2);
  printf("test_size=%d test_iterations=%d ", TEST_SIZE, TEST_ITERATIONS);
  printf("time=%lf arity=%u\n", tdiff(t2,t1), ARITY);
}

