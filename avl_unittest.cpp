#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
// This turns on rather expensive internal consistancy checking
#define AVL_DEBUG
#include "avl.h"

#define TEST_SIZE 500

class AVLNode: public AVLNode_base<AVLNode, int> {
  public:
    int value;
  public:
    AVLNode(int v) {
      value = v;
    }
    const int val(void) {
      return value;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    void print(void) {
      printf("%d,%d", value, (int) balance);
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

void print_tree(AVL<AVLNode, int> *t) {
    t->print();
}

void check(AVL<AVLNode, int> *tree, TrivialDict<TrivialDictDatum,int> *dict) {
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
  AVL<AVLNode, int> tree;

  int i;
  // insert in order, then remove
  printf("Begin AVL.h test\n");
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
      AVLNode *n = new AVLNode(r);
      tree.insert(n);
      // and in the list
      dict.insert(r);
      // check that everything is in the list that should be
      check(&tree, &dict);
    }
    while (!dict.isempty()) {
      // We're invalidating our iterator every round, by modifying the tree
      auto i = dict.begin();
      auto n = tree.get(i->val());
      tree.remove(n);
      delete n;
      dict.remove(i->val());
      // check that everything is in the list that should be
      check(&tree, &dict);
    }
    if (!tree.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
  }
  
  // Check non-existant element
  AVLNode *n = tree.get(1); 
  if (n != nullptr) {
    PANIC("Underflow returned a value!");
  }

  // Check duplicate element behavior
  n = new AVLNode(1);
  bool inserted = tree.insert(n);
  if (!inserted) {
    PANIC("Insertion failed");
  }
  n = new AVLNode(1);
  inserted = tree.insert(n);
  if (inserted) {
    PANIC("Double insertion succeeded");
  }
  n = tree.get(1);
  if (n == nullptr) {
    PANIC("Attempted insertion removed existing data");
  }
  n = tree.get(1);
  tree.remove(n);
  delete n;
  n = tree.get(1);
  if (n != nullptr) {
    PANIC("underflow returned a value after duplicate insert");
  }

  printf("PASS\n");
}
