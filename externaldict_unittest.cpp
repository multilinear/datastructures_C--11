/* Copywrite Matthew Brewer 2017-03-22
 *
 * This is a unittest for dictionaries using external allocation
 * We decide WHAT we're testing using the macro system
 * Our Makefile takes advantage of this.
 *
 * This seems odd, but it avoids code duplication and ensures all our
 * datastructures have the same API (Except where it really isn't a good idea)
 */

#include <stdio.h>
#include "panic.h"
#include "dlist.h"

//#define DEBUG_OUTPUT

#define TEST_SIZE 200

#ifdef TEST_AVL
// This turns on rather expensive internal consistancy checking
#define DEBUG_AVL
#include "avl.h"
#define SORTED_ITERATOR
class Node: public AVLNode_base<Node, int> {
#endif
#ifdef TEST_AVLHASHTABLE
// This turns on rather expensive internal consistancy checking
#define DEBUG_AVLHASHTABLE
#include "avlhashtable.h"
class Node: public AVLHashTableNode_base<Node, int> {
#endif
#ifdef TEST_BOUNDEDHASHTABLE
// This turns on rather expensive internal consistancy checking
#define DEBUG_BoundedHASHTABLE
#include "boundedhashtable.h"
class Node: public BoundedHashTableNode_base<Node, int> {
#endif

#ifdef TEST_OCHASHTABLE
// This turns on rather expensive internal consistancy checking
#define DEBUG_OCHASHTABLE
#include "ochashtable.h"
class Node: public OCHashTableNode_base<Node> {
#endif
#ifdef TEST_REDBLACK
// This turns on rather expensive internal consistancy checking
#define DEBUG_REDBLACK
#include "redblack.h"
#define SORTED_ITERATOR
class Node: public RedBlackNode_base<Node, int> {
#endif
#ifdef TEST_RREDBLACK
// This turns on rather expensive internal consistancy checking
#define DEBUG_RREDBLACK
#include "rredblack.h"
#define SORTED_ITERATOR
class Node: public RRedBlackNode_base<Node, int> {
#endif
  public:
    int value;
  public:
    const int val(void) const {
      return value;
    }
    static size_t hash(int v) {
      return v;
    }
    static int compare(const int v1, const int v2) {
      return v1-v2;
    }
    Node() {}
    void set(int v) {
      value = v;
    }
    Node(int v) {
      value = v;
    }
    void print(void) const {
      printf("%d", value);
    }
};

class TNode: public DListNode_base<TNode> {
  public:
    int value;
  public:
    TNode() { }
    TNode(int i) {
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

template<typename DT>
void check(DT *dict, DList<TNode,int> *tdict) {
  auto i = tdict->begin();
  for (; i != tdict->end(); i++) {
    if(!dict->get(i->val())) {
      printf("value = %d\n", i->val());
      PANIC("Element not in dict anymore!");
    }
  }
  auto j = dict->begin();
  for (; j != dict->end(); j++) {
    if (!tdict->get(j->val())) {
      printf("value = %d\n", j->val());
      PANIC("Element in dict, but should be!");
    }
  }
}

int main(int argc, char* argv[]) {
  DList<TNode, int> tdict;
  #ifdef TEST_OCHASHTABLE
  printf("Begin OCHashTable.h unittest\n");
  OCHashTable<Node, int> dict;
  #endif
  #ifdef TEST_AVLHASHTABLE
  printf("Begin AVLHashTable.h unittest\n");
  AVLHashTable<Node, int> dict;
  #endif
  #ifdef TEST_AVL
  printf("Begin AVL.h unittest\n");
  AVL<Node, int> dict;
  #endif
  #ifdef TEST_BOUNDEDHASHTABLE
  printf("Begin BoundedHashTable.h unittest\n");
  BoundedHashTable<Node, int> dict;
  #endif
  #ifdef TEST_REDBLACK
  printf("Begin RedBlack.h unittest\n");
  RedBlack<Node, int> dict;
  #endif
  #ifdef TEST_RREDBLACK
  printf("Begin RRedBlack.h unittest\n");
  RRedBlack<Node, int> dict;
  #endif

  int i;
  // insert in order, then remove
  int k;
  for (k=1; k<TEST_SIZE; k++) {
    for (i=0; i<k; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = true;
        new_v = !tdict.get(r);
      }
      #ifdef DEBUG_OUTPUT
      printf("Inserting %d\n", r);
      #endif
      // put it in thet dict
      Node *n = new Node(r);
      dict.insert(n);
      // and in the list
      tdict.insert(new TNode(r));
      // check that everything is in the list that should be
      #ifdef DEBUG_OUTPUT
      printf("tdict=");
      tdict.print();
      printf("\ndict=");
      dict.print();
      #endif
      check<decltype(dict)>(&dict, &tdict);
    }
    while (!tdict.isempty()) {
      // We're invalidating our iterator every round, by modifying the dict
      auto i = tdict.begin();
      #ifdef DEBUG_OUTPUT
      printf("Removing %d\n", i->val());
      #endif
      auto n = dict.get(i->val());
      if (!n) {
        PANIC("Node not found\n");
      }
      #ifdef TEST_RREDBLACK
      dict.remove(i->val());
      #else
      n = dict.get(i->val());
      dict.remove(n);
      delete n;
      #endif
      auto tn = &(*i);
      tdict.remove(tn);
      delete tn;
      // check that everything is in the list that should be
      #ifdef DEBUG_OUTPUT
      printf("tdict=");
      tdict.print();
      printf("\ndict=");
      dict.print();
      #endif
      check<decltype(dict)>(&dict, &tdict);
    }
    if (!dict.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
  }
  
  // Check non-existant element
  Node *n = dict.get(1); 
  if (n != nullptr) {
    PANIC("Underflow returned a value!");
  }

  // Check duplicate element behavior
  n = new Node(1);
  bool inserted = dict.insert(n);
  if (!inserted) {
    PANIC("Insertion failed");
  }
  #ifndef TEST_RREDBLACK
  n = new Node(1);
  inserted = dict.insert(n);
  if (inserted) {
    PANIC("Double insertion succeeded");
  }
  #endif
  // We didn't bother making rredblack handle double insertion elegantly
  n = dict.get(1);
  if (n == nullptr) {
    PANIC("Attempted insertion removed existing data");
  }
  n = dict.get(1);
  // RRedblack has no reason to do node based removal, value-based is just as fast
  #ifdef TEST_RREDBLACK
  dict.remove(n->val());
  #else
  dict.remove(n);
  delete n;
  #endif
  n = dict.get(1);
  if (n != nullptr) {
    PANIC("underflow returned a value after duplicate insert");
  }
  // Test SORTED iterator
  #ifdef SORTED_ITERATOR
  for (size_t i=0; i<100; i++) {
    dict.insert(new Node(i));
  }
  i = 0;
  for (auto i2 = dict.begin(); i2 != dict.end(); ++i2) {
    if (i2->val() != i) {
      dict.print();
      printf("%d should be %d\n", i2->val(), i);
      PANIC("dict iterator broken");
    }
    ++i;
  }
  // drain the dictionary
  auto a = dict.begin();
  while (a != dict.end()) {
    #ifdef TEST_RREDBLACK
    dict.remove(a->val());
    #else
    dict.remove(&(*a));
    delete (&(*a));
    #endif
    a = dict.begin();
  }
  #endif
  printf("PASS\n");
}
