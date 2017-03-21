/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A standard open chaining hashtable implementation, using external allocation
 * for nodes.
 *
 * resizes up when size is < 2x data it contains
 * resizes down when size is > 4x data it contains
 *
 * Faster average case than btree by ~2x
 *
 * Worst case operation is linear per op due to
 * 1) linear rehash
 * 2) possability of every item hash colliding
 */ 

// TODO: Add an iterator

#include "panic.h"
#include "array.h"
#include "dlist.h"

#ifndef OC_HASHTABLE_H
#define OC_HASHTABLE_H

#define MINSIZE 16

template <typename Node_T>
class OCHashTableNode_base: public DListNode_base<Node_T> {
  public:
    // The size when this was hashed, used like a generation counter
    // So we can track what was rehashed during a resize
    size_t hs;
    // subclass must implement:
    // Val_T val(void);
    // static size_t hash(Val_T v);
};

template <typename Node_T, typename Val_T>
class OCHashTable {
  private:
    Array<DList<Node_T>> table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
    OCHashTable();
    OCHashTable(size_t s);
    ~OCHashTable();
    bool insert(Node_T *n);
    Node_T* get(Val_T key);
    Node_T* remove(Node_T *n);
    bool isempty(void) const; 
    void resize(size_t s);
};

template <typename Node_T, typename Val_T>
OCHashTable<Node_T,Val_T>::OCHashTable():table(MINSIZE) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<MINSIZE; ++i) {
    table[i] = DList<Node_T>();
  }
}

template <typename Node_T, typename Val_T>
OCHashTable<Node_T,Val_T>::OCHashTable(size_t s):table(s) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<s; ++i) {
    table[i] = DList<Node_T>();
  }
}

template <typename Node_T, typename Val_T>
OCHashTable<Node_T,Val_T>::~OCHashTable() {
  for (size_t i=0; i<table.len(); ++i) {
    if (!table[i].isempty()) {
      PANIC("Hashtable not empty before destruction");
    }
    // explicitly destruct since array doesn't do that
    table[i].~DList();
  }
};

template <typename Node_T, typename Val_T>
bool OCHashTable<Node_T,Val_T>::insert(Node_T *new_node) {
  check_sizeup();
  Val_T v = new_node->val();
  new_node->hs = table.len();
  size_t i = Node_T::hash(v) % table.len();
  // This is just to check for repeat keys
  // if we allow repeat keys we can remove this
  // TODO: maybe everything should have a "contains" operator?
  for (auto n = table[i].begin(); n != table[i].end(); ++n) {
    // We check v rather than hash, this way correctness
    // is preserved if hashes collide
    if (n->val() == v) {
      return false;
    }
  } 
  table[i].enqueue(new_node);
  count++;
  return true;
}

template <typename Node_T, typename Val_T>
Node_T* OCHashTable<Node_T,Val_T>::get(Val_T key) {
  size_t i = Node_T::hash(key) % table.len();
  for (auto n = table[i].begin(); n != table[i].end(); ++n) {
    // We check v rather than hash, this way correctness
    // is preserved if hashes collide
    if (n->val() == key) {
      return &(*n);
    }
  }
  return nullptr;
}

template <typename Node_T, typename Val_T>
Node_T * OCHashTable<Node_T,Val_T>::remove(Node_T *n) {
  // Note, if n is not in the hashtable, this will cause
  // some nasty corruption.
  Val_T v = n->val();
  size_t i = Node_T::hash(v) % table.len();
  table[i].remove(&(*n)); 
  count--;
  check_sizedown();
  return n;
}

template <typename Node_T, typename Val_T>
bool OCHashTable<Node_T,Val_T>::isempty(void) const {
  return count == 0;
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::resize(size_t s) {
  // nothing to do
  if (s == table.len()) {
    return;
  }
  // If increasing size, we resize before we rehash
  if (s > table.len()) {
    size_t old_size = table.len();
    table.resize(s);
    // We have to initialize the lists since
    // array doesn't construct objects it contains
    for (size_t i=old_size; i<s; ++i) {
      table[i] = DList<Node_T>(); 
    }
  }
  // Rehash everything based on "s"
  for (size_t i=0; i<table.len(); ++i) {
    auto n = table[i].begin();
    while (n != table[i].end() && n->hs != s) {
      if (n->hs != s) {
        auto node = &(*n);
        table[i].remove(node);
        size_t new_index = Node_T::hash(node->val()) % s;
        node->hs = s;
        table[new_index].enqueue(node);
        n = table[i].begin();
      } else {
        n++;
      }
    }
  }
  // If decreasing we resize after
  if (s < table.len()) {
    table.resize(s);
  }
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::check_sizedown(void) {
  // If it's under a quarter full resize down
  if (table.len() > 4*count) {
    size_t s = table.len() / 2;
    if (s < MINSIZE) {
      s = MINSIZE;
    }
    resize(s); 
  }
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table.len() < 2*count) {
    resize(table.len()*2); 
  } 
}

#endif
