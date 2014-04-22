#include <stdio.h>
#include "panic.h"
// This turns on rather expensive internal consistancy checking
#define REDBLACK_DEBUG
#include "redblack.h"

class RedBlackNode: public RedBlackNode_base<RedBlackNode, int> {
  public:
    int value;
  public:
    const int val(void) {
      return value;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    RedBlackNode(int v) {
      value = v;
    }
    void print(void) {
      printf("%d,%d", value, red);
    }
};

void pre_print(RedBlackNode *n) {
  printf("(%d,%d|", n->value, n->red);
}

void post_print(RedBlackNode *n) {
  printf(")");
}

void leaf_print() {
  printf("n");
}

void print_tree(RedBlack<RedBlackNode, int> *t) {
    //t->order(pre_print, nullptr, post_print, leaf_print);
    t->print();
}

#define TEST_SIZE 500
int ints[TEST_SIZE];
int ints_start;
int ints_end;

int check(RedBlack<RedBlackNode, int> *tree) {
  int i;
  for (i=ints_start; i<ints_end; i++) {
    if(!tree->get(ints[i])) {
      printf("%d\n", ints[i]);
      PANIC("Element not in tree anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  RedBlack<RedBlackNode, int> tree;

  int i;
  int j;
  // insert in order, then remove
  printf("Begin RedBlack.h test\n");
  for (j=0; j<TEST_SIZE; j++) {
    for (i=0; i<j; i++) {
      RedBlackNode *n = new RedBlackNode(i);
      tree.insert(n);
    }
    for (i=0; i<j; i++) {
      delete tree.remove(tree.get(i));
    }
    if (!tree.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
  }
  for (j=1; j<TEST_SIZE; j++) {
    // insert in reverse order, then remove in reverse order
    for (i=j; i>0; i--) {
      RedBlackNode *n = new RedBlackNode(i);
      tree.insert(n);
    }
    for (i=j; i>0; i--) {
      delete tree.remove(tree.get(i));
    }
  }
  int k;
  for (k=1; k<TEST_SIZE; k++) {
    ints_end=0;
    ints_start=0;
    for (i=0; i<k; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = true;
        for (j=ints_start; j<ints_end; j++) {
          if (ints[j] == r){
            new_v = false;
            break;
          }
        }
      }
      // put it in thet tree
      RedBlackNode *n = new RedBlackNode(r);
      tree.insert(n);
      // and in the list
      ints[ints_end++] = r;
      // check that everything is in the list that should be
      check(&tree);
    }
    for(i=0; i<ints_end; i++) {
      RedBlackNode *n = tree.get(ints[i]);
      delete tree.remove(tree.get(ints[i]));
      ints_start += 1;
      // check that everything is in the list that should be
      check(&tree);
    }
    if (!tree.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
  }
  printf("PASS\n");
}
