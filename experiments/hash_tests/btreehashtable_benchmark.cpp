#include <stdio.h>
#include "panic.h"
#include "btreehashtable.h"

#define TEST_ITERATIONS 1000
#define TEST_SIZE 100000

class Comp { 
  public:
    // function to extract val from node
    // return type must be comparable
    // value must be unique for nodes that should be unique
    static const int val(const int v) {
      return v;
    } 
    // function to compare to values
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    // function to hash val
    // this should be well distributed, but uniqueness is not necessary
    static size_t hash(int v) {
      return v;
    }

    static void printT(int d) {
      printf("%d",d);
    } 
};

int ints[TEST_SIZE];
int ints_end;

int main(int argc, char* argv[]) {
  BTreeHashTable<int, int, Comp> hash;

  int j;
  int get_count=0;
  printf("Begin BTreeHashTable.h benchmark\n");
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
        new_v = !hash.get(r);
        get_count++;
      }
      // put it in thet tree
      hash.insert(r);
      // and in the list
      ints[ints_end++] = r;
    }
    for(i=0; i<ints_end; i++) {
      hash.remove(ints[i]);
    }
  }
  printf("get_count %d\n", get_count);

}
