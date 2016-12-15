#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
// This turns on rather expensive internal consistancy checking
#define BTREE_DEBUG
// This turns on the iterator (adding a parent pointer)
#define BTREE_ITERATOR
#include "btree.h"

#define TEST_SIZE 200
#define ARITY 5

class BTreeComp {
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

void print_tree(BTree<int,int,BTreeComp,ARITY> *t) {
    t->print();
}

void check(BTree<int,int,BTreeComp,ARITY> *tree, TrivialDict<TrivialDictDatum,int> *dict) {
  auto i = dict->begin();
  int d;
  for (; i != dict->end(); i++) {
    if(!tree->get(i->val())) {
      printf("%d\n", i->val());
      PANIC("Element not in tree anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum, int> dict(TEST_SIZE);
  BTree<int,int,BTreeComp,ARITY> tree;

  int i;
  // insert in order, then remove
  printf("Begin BTree.h test\n");
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
      int d;
      bool f;
      //printf("removing %d\n", i->val());
      f = !!tree.get(i->val());
      if (!f) {
        PANIC("tree lacks element it should have");
      }
      tree.remove(i->val(), &d);
      f = !!tree.get(i->val());
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
  // Test iterator
  for (size_t i=0; i<100; i++) {
    tree.insert(i);
  }
  i = 0;
  for (auto i2 = tree.begin(); i2 != tree.end(); ++i2) {
    if (*i2 != i) {
      PANIC("Tree iterator broken");
    }
    ++i;
  }
  int val;
  auto a = tree.begin();
  while (a != tree.end() && tree.remove(*a, &val)) {
    a = tree.begin();
  }
  printf("PASS\n");
}
