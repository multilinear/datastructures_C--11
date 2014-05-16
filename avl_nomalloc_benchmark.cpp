#include <stdio.h>
#include "panic.h"
#include "avl.h"

#define TEST_SIZE 20000000

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
    void print(void) {
    }
};


int ints[TEST_SIZE];
int ints_start;
int ints_end;

int main(int argc, char* argv[]) {
  AVL<AVLNode, int> tree;
  AVLNode *elements = new AVLNode[TEST_SIZE];
  int used = 0;
  int i;
  int j;
  int gets=0;
  printf("Begin AVL.h benchmark\n");
  ints_end=0;
  ints_start=0;
  for (i=0; i<TEST_SIZE; i++) {
    bool new_v = false;
    int r;
    // find a value we haven't used yet
    while (!new_v) {
      // Note, we did not initialize rand, this is purposeful
      r = rand();
      new_v = (tree.get(r) == nullptr); 
      gets++;
    }
    // put it in thet tree
    AVLNode *n = &(elements[used++]);
    n->value = r;
    tree.insert(n);
    // and in the list
    ints[ints_end++] = r;
  }
  for(i=0; i<ints_end; i++) {
    tree.remove(tree.get(ints[i]));
    ints_start += 1;
  }
  printf("gets %d\n", gets);
}
