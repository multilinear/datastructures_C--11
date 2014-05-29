#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
// This turns on rather expensive internal consistancy checking
#define TSBTREE_DEBUG
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
  int d;
  for (; i != dict->end(); i++) {
    if(!tree->get(i->val(), &d)) {
      printf("%d\n", i->val());
      PANIC("Element not in tree anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum, int> dict(TEST_SIZE);
  TSBTree<int,int,TSBTreeComp,ARITY> tree;

  int i;
  // insert in order, then remove
  printf("Begin TSBTree.h test\n");
  int k;
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
        TrivialDictDatum d;
        new_v = !dict.get(r, &d);
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
      int d;
      bool f;
      //printf("removing %d\n", i->val());
      f = tree.get(i->val(), &d);
      if (!f) {
        PANIC("tree lacks element it should have");
      }
      tree.remove(i->val(), &d);
      f = tree.get(i->val(), &d);
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
  printf("PASS\n");
  return 0;
}
