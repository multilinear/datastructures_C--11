#include <stdio.h>
#include "panic.h"
#include "ts_btree.h"

#define ARITY 5
#define TEST_SIZE 20000000

class TSBTreeComp {
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
int ints_start;
int ints_end;

int main(int argc, char* argv[]) {
  TSBTree<int, int, TSBTreeComp, ARITY> tree;

  int i;
  int j;
  int gets=0;
  printf("Begin TSBTree.h benchmark\n");
  ints_end=0;
  ints_start=0;
  for (i=0; i<TEST_SIZE; i++) {
    bool new_v = false;
    int r;
    // find a value we haven't used yet
    while (!new_v) {
      // Note, we did not initialize rand, this is purposeful
      r = rand();
      int junk;
      new_v = !tree.get(r, &junk); 
      gets++;
    }
    // put it in thet tree
    tree.insert(r);
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
