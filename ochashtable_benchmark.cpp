#include <stdio.h>
#include "panic.h"
#include "ochashtable.h"

#define TEST_ITERATIONS 1000000
#define TEST_SIZE 100

class HNode: public OCHashTableNode_base<HNode> {
  public:
    int value;
  public:
    const int val(void) {
      return value;
    }
    static size_t hash(int v) {
      return v;
    }
    HNode() {}
    void set(int v) {
      value = v;
    }
    HNode(int v) {
      value = v;
    }
    void print(void) {
    }
};


int ints[TEST_SIZE];
int ints_end;

// necessary for external allocation datastructures
HNode *nodes = new HNode[TEST_SIZE];

int main(int argc, char* argv[]) {
  OCHashTable<HNode, int> hash;

  int j;
  int get_count=0;
  printf("Begin OCHashTable.h benchmark\n");
  for (j=0; j<TEST_ITERATIONS; j++) {
    ints_end=0;
    int i;
    int ni=0;
    for (i=0; i<TEST_SIZE; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = (!hash.get(r)); 
        get_count++;
      }
      // put it in thet hash
      HNode *n = &(nodes[ni++]); 
      n->set(r);
      hash.insert(n);
      // and in the list
      ints[ints_end++] = r;
    }
    for(i=0; i<ints_end; i++) {
      int v = ints[i];
      auto n = hash.get(v);
      hash.remove(n);
    }
  }
  printf("get_count %d\n", get_count);
}
