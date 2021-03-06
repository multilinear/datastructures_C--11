#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
// This turns on rather expensive internal consistancy checking
#define TSBTREE_DEBUG
//#define TSBTREE_DEBUG_VERBOSE
#include "ts_btree.h"

#define TEST_SIZE 200
#define ARITY 5

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

void print_tree(TSBTree<int,int,TSBTreeComp,ARITY> *t) {
    t->print();
}

void check(TSBTree<int,int,TSBTreeComp,ARITY> *tree, TrivialDict<TrivialDictDatum,int> *dict) {
  auto i = dict->begin();
  int val;
  for (; i != dict->end(); i++) {
    if(!tree->get(i->val(), &val)) {
      printf("%d\n", i->val());
      PANIC("Element not in tree anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum, int> dict(TEST_SIZE);
  TSBTree<int,int,TSBTreeComp,ARITY> tree;

  // insert in order, then remove
  printf("Begin TSBTree.h test\n");
  int i;
  int k;
  int val;
  for (k=1; k<TEST_SIZE; k++) {
    dict.reset(TEST_SIZE);
    //printf("**** Adding elements\n");
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
      tree.insert(r);
      dict.insert(r);
      check(&tree, &dict);
    }
    //printf("**** Draining\n");
    while (!dict.isempty()) {
      // We're invalidating our iterator every round, by modifying the tree
      // but we removed the first element, so we just start over again
      auto i = dict.begin();
      bool f;
      f = !!tree.get(i->val(), &val);
      if (!f) {
        PANIC("tree lacks element it should have");
      }
      tree.remove(i->val(), &val);
      f = tree.get(i->val(), &val);
      if (f) {
        printf("Element %d\n", i->val());
        PANIC("tree has element it should not have");
      }
      dict.remove(i->val());
      check(&tree, &dict);
    }
    //printf("Checking  that things are empty\n");
    if (!tree.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
    //printf("************ size %d complete\n", k);
  }
  // Check duplicate behavior
  if (!tree.insert(1))  {
    PANIC("Tree insert of single value failed");
  }
  if (tree.insert(1)) {
    PANIC("Tree insert of duplicate succeeded");
  }
  if (tree.isempty()) {
    PANIC("Tree thinks it's empty");
  }
  int v;
  if (!tree.remove(1, &v)) {
    PANIC("Tree remove of first after duplicate insert failed");
  }
  if (tree.remove(1, &v)) {
    PANIC("Tree remove of first after duplicate insert failed");
  }
  if (!tree.isempty()) {
    PANIC("Tree thinks it's not empty");
  }

  printf("PASS\n");
  return 0;
}
