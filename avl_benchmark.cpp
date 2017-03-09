#include <stdio.h>
#include "panic.h"
#include "avl.h"

#define TEST_ITERATIONS 1000000
#define TEST_SIZE 100

class AVLNode: public AVLNode_base<AVLNode, int> {
  public:
    int value;
  public:
    const int val(void) {
      return value;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    AVLNode() {}
    void set(int v) {
      value = v;
    }
    AVLNode(int v) {
      value = v;
    }
    void print(void) {
    }
};


int ints[TEST_SIZE];
int ints_end;

int main(int argc, char* argv[]) {
  AVL<AVLNode, int> tree;
  printf("Begin AVL.h benchmark\n");
  int j;
  int get_count=0;
  AVLNode *nodes = new AVLNode[TEST_SIZE];
  for (j=0;j<TEST_ITERATIONS;j++) {
    int i;
    ints_end=0;
    int ni=0;
    for (i=0; i<TEST_SIZE; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = (tree.get(r) == nullptr); 
        get_count++;
      }
      // put it in thet tree
      AVLNode *n = &(nodes[ni++]); 
      n->set(r);
      tree.insert(n);
      // and in the list
      ints[ints_end++] = r;
    }
    for(i=0; i<ints_end; i++) {
      int v = ints[i];
      auto n = tree.get(v);
      tree.remove(n);
    }
  }
  printf("get_count %d\n", get_count);
}
