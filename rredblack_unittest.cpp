#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
// This turns on rather expensive internal consistancy checking
#define RREDBLACK_DEBUG
#include "rredblack.h"

#define TEST_SIZE 500

class RRedBlackNode: public RRedBlackNode_base<RRedBlackNode, int> {
  public:
    int value;
  public:
    RRedBlackNode(int v) {
      value = v;
    }
    const int val(void) {
      return value;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    void print(void) {
      if (red) {
        printf("%d,R", value);
      } else {
        printf("%d,B", value);
      }
    }
};

class TrivialDictDatum {
  public:
    int value;
  public:
    TrivialDictDatum() { }
    TrivialDictDatum(int i) {
      value = i;
    }
    const int val(void) {
      return value;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    void print(void) {
      printf("%d", value);
    }
};

void print_tree(RRedBlack<RRedBlackNode, int> *t) {
    t->print();
}

void check(RRedBlack<RRedBlackNode, int> *tree, TrivialDict<TrivialDictDatum,int> *dict) {
  auto i = dict->begin();
  for (; i != dict->end(); i++) {
    if(!tree->get(i->val())) {
      printf("%d\n", i->val());
      PANIC("Element not in tree anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum, int> dict(TEST_SIZE);
  RRedBlack<RRedBlackNode, int> tree;

  int i;
  // insert in order, then remove
  printf("Begin RRedBlack.h test\n");
  int k;
  for (k=1; k<TEST_SIZE; k++) {
    dict.reset(TEST_SIZE);
    for (i=0; i<k; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = true;
        new_v = !dict.get(r);
      }
      // put it in thet tree
      RRedBlackNode *n = new RRedBlackNode(r);
      tree.insert(n);
      // and in the list
      dict.insert(r);
      // check that everything is in the list that should be
      check(&tree, &dict);
    }
    while (!dict.isempty()) {
      // We're invalidating our iterator every round, by modifying the tree
      auto i = dict.begin();
      delete tree.remove(i->val());
      dict.remove(i->val());
      // check that everything is in the list that should be
      check(&tree, &dict);
    }
    if (!tree.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
  }
  // We do no duplicate testcase, because RRedBlack doesn't currently support it
  // It simply panics if it hits a duplicate
  printf("PASS\n");
}
