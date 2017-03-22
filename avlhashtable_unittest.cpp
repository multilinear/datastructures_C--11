// This turns on rather expensive internal consistancy checking
#define AVLHASHTABLE_DEBUG
#define ARRAY_DEBUG
#define DLIST_DEBUG
#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"
#include "avlhashtable.h"

#define TEST_SIZE 500

class AVLHashTableNode: public AVLHashTableNode_base<AVLHashTableNode, int> {
  public:
    int v;
    // function to extract val from node
    // return type must be comparable
    // value must be unique for nodes that should be unique
    int val(void) {
      return v;
    }
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
    // function to hash val
    // this should be well distributed, but uniqueness is not necessary
    static size_t hash(int v) {
      return v;
    }
    AVLHashTableNode(int val) {
      v = val;
    }
    // This override is optional, we just add it for debugging
    void print(void) {
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

void check(AVLHashTable<AVLHashTableNode, int> *hash, TrivialDict<TrivialDictDatum,int> *dict) {
  auto i = dict->begin();
  for (; i != dict->end(); i++) {
    if(!hash->get(i->val())) {
      printf("value = %d\n", i->val());
      //printf("Element not in hashtable anymore!\n");
      PANIC("Element not in hashtable anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum, int> dict(TEST_SIZE);
  AVLHashTable<AVLHashTableNode, int> hash;

  int i;
  // insert in order, then remove
  printf("Begin AVLHashTable.h test\n");
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
      // put it in the hashtable
      hash.insert(new AVLHashTableNode(r));
      // and in the list
      dict.insert(r);
      // check that everything is in the list that should be
      check(&hash, &dict);
    }
    while (!dict.isempty()) {
      // We're invalidating our iterator every round, by modifying the tree
      auto i = dict.begin();
      hash.remove(hash.get(i->val()));
      dict.remove(i->val());
      // check that everything is in the list that should be
      check(&hash, &dict);
    }
    if (!hash.isempty()) {
      PANIC("Tree should be empty here, but isn't");
    }
  }
  
  // Check non-existant element
  AVLHashTableNode *n = hash.get(1); 
  if (n != nullptr) {
    PANIC("Underflow returned a value!");
  }

  // Check duplicate element behavior
  bool inserted = hash.insert(new AVLHashTableNode(1));
  if (!inserted) {
    PANIC("Insertion failed");
  }
  inserted = hash.insert(new AVLHashTableNode(1));
  if (inserted) {
    PANIC("Double insertion succeeded");
  }
  n = hash.get(1);
  if (n == nullptr) {
    PANIC("Attempted insertion removed existing data");
  }
  hash.remove(hash.get(1));
  n = hash.get(1);
  if (n != nullptr) {
    PANIC("underflow returned a value after duplicate insert");
  }

  printf("PASS\n");
}
