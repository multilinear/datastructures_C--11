#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
// This turns on rather expensive internal consistancy checking
#define BTREE_DEBUG
#include "btree.h"

#define TEST_SIZE 500
#define ARITY 5

class BTreeDatum {
  public:
    int value;
  public:
    BTreeDatum(){}
    BTreeDatum(int v) {
      value = v;
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

void print_tree(BTree<BTreeDatum,int,ARITY> *t) {
    t->print();
}

int check(BTree<BTreeDatum,int,ARITY> *tree, TrivialDict<TrivialDictDatum,int> *dict) {
  auto i = dict->begin();
  BTreeDatum d;
  for (; i != dict->end(); i++) {
    if(!tree->get(i->val(), &d)) {
      printf("%d\n", i->val());
      PANIC("Element not in tree anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum, int> dict(TEST_SIZE);
  BTree<BTreeDatum,int,ARITY> tree;

  int i;
  int j;
  // insert in order, then remove
  printf("Begin BTree.h test\n");
  int k;
  for (k=1; k<TEST_SIZE; k++) {
    printf("************ testing size %d\n", k);
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
      tree.insert(BTreeDatum(r));
      dict.insert(r);
      check(&tree, &dict);
    }
    //printf("**** Draining\n");
    while (!dict.isempty()) {
      // We're invalidating our iterator every round, by modifying the tree
      // but we removed the first element, so we just start over again
      auto i = dict.begin();
      BTreeDatum d;
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
}
