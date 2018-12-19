#include <stdio.h>
#include "panic.h"
#include "dlist.h"

#define TEST_SIZE 200

#ifdef TEST_BTREE
// This turns on rather expensive internal consistancy checking
#define BTREE_DEBUG
// This turns on the iterator (adding a parent pointer)
#define BTREE_ITERATOR
#include "btree.h"
#define ARITY 5
#endif

#ifdef TEST_TS_BTREE
#define BTREE_DEBUG
#include "ts_btree.h"
#define ARITY 5
#endif

#ifdef TEST_BTREEHASHTABLE
#include "btreehashtable.h"
#endif

#ifdef TEST_HASHTABLE
#include "hashtable.h"
#endif

class Comp {
  // For use with T=int, Val_T=int
  public:
    static const int val(const int T) {
      return T;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    static size_t hash(int v) {
      return v;
    }
    static void printT(const int t) {
      printf("%d", t);
    }
    static void printV(const int v) {
      printf("%d", v);
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
    #ifdef TEST_TS_BTREE
    int res;
    if(!dict->get(i->val(), &res)) {
    #else
    if(!dict->get(i->val())) {
    #endif
      printf("%d\n", i->val());
      PANIC("Element not in dict anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  DList<TNode, int> tdict;

  #ifdef TEST_BTREE
  printf("Begin BTree.h unittest\n");
  BTree<int, int, Comp, ARITY> dict;
  #endif
  #ifdef TEST_TS_BTREE
  printf("Begin TS_BTree.h unittest\n");
  TSBTree<int, int, Comp, ARITY> dict;
  #endif
  #ifdef TEST_BTREEHASHTABLE
  printf("Begin BTreeHashTable.h unittest\n");
  BTreeHashTable<int, int, Comp> dict;
  #endif
  #ifdef TEST_HASHTABLE
  printf("Begin HashTable.h unittest\n");
  HashTable<int, int, Comp> dict;
  #endif

  int i;
  // insert in order, then remove
  int k;
  for (k=1; k<TEST_SIZE; k++) {
    //printf("**** Adding elements\n");
    //dict.print();
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
      dict.insert(r);
      tdict.insert(new TNode(r));
      check<decltype(dict)>(&dict, &tdict);
      //dict.print();
    }
    //printf("**** Draining\n");
    //dict.print();
    while (!tdict.isempty()) {
      // We're invalidating our iterator every round, by modifying the dict
      // but we removed the first element, so we just start over again
      auto i = tdict.begin();
      int d;
      bool f;
      //printf("removing %d\n", i->val());
      #ifdef TEST_TS_BTREE
      int res;
      f = !!dict.get(i->val(), &res);
      #else
      f = !!dict.get(i->val());
      #endif
      if (!f) {
        PANIC("dict lacks element it should have");
      }
      dict.remove(i->val(), &d);
      #ifdef TEST_TS_BTREE
      f = !!dict.get(i->val(), &res);
      #else
      f = !!dict.get(i->val());
      #endif
      if (f) {
        printf("Element %d\n", i->val());
        PANIC("dict has element it should not have");
      }
			auto tn = &(*i);
      tdict.remove(tn);
			delete tn;
      check<decltype(dict)>(&dict, &tdict);
      //dict.print();
    }
    //printf("Checking  that things are empty\n");
    if (!dict.isempty()) {
      dict.print();
      PANIC("Dict should be empty here, but isn't");
    }
    //printf("************ size %d complete\n", k);
  }
  // Check duplicate behavior
  if (!dict.insert(1))  {
    PANIC("Dict insert of single value failed");
  }
  if (dict.insert(1)) {
    PANIC("Dict insert of duplicate succeeded");
  }
  if (dict.isempty()) {
    PANIC("Dict thinks it's empty");
  }
  int v;
  if (!dict.remove(1, &v)) {
    PANIC("Dict remove of first after duplicate insert failed");
  }
  if (dict.remove(1, &v)) {
    PANIC("Dict remove of duplicate succeeded");
  }
  if (!dict.isempty()) {
    dict.print();
    PANIC("Dict thinks it's not empty");
  }
  #ifndef TEST_TS_BTREE
  // Test iterator
  for (size_t i=0; i<100; i++) {
    dict.insert(i);
  }
  i = 0;
  for (auto i2 = dict.begin(); i2 != dict.end(); ++i2) {
    if (*i2 != i) {
      printf("%d should be %d\n", *i2, i);
      PANIC("dict iterator broken");
    }
    ++i;
  }
  int val;
  auto a = dict.begin();
  while (a != dict.end() && dict.remove(*a, &val)) {
    a = dict.begin();
  }
  for (auto i2 = dict.begin(); i2 != dict.end(); ++i2) {
    PANIC("Iterator returning elements from empty structure");
  }
  #endif
  printf("PASS\n");
}
