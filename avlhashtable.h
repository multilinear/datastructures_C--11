/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A standard open chaining hashtable implementation, using external allocation
 * for nodes.... EXCEPT that we use an avl tree instead of a linked-list
 *
 * resizes up when size is < 2x data it contains
 * resizes down when size is > 4x data it contains
 *
 * Worst case operation is linear per op due to
 * 1) linear rehash
 */ 

// TODO: Add an iterator

#include <stdio.h>
#include <utility>
#include "panic.h"
#include "array.h"
#include "avl.h"

#ifndef AVL_HASHTABLE_H
#define AVL_HASHTABLE_H

#define MINSIZE 16

template <typename Node_T, typename Val_T>
class AVLHashTableNode_base: public AVLNode_base<Node_T, Val_T> {
  public:
    // The size when this was hashed, used like a generation counter
    // So we can track what was rehashed during a resize
    size_t hs;
    // subclass must implement:
    // Val_T val(void);
    // void print(void);
    // static size_t hash(Val_T v);
};

template <typename Node_T, typename Val_T>
class AVLHashTable {
  private:
    Array<AVL<Node_T, Val_T>> table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
    AVLHashTable();
    AVLHashTable(size_t s);
    ~AVLHashTable();
    bool insert(Node_T *n);
    Node_T* get(Val_T key);
    Node_T* remove(Node_T *n);
    bool isempty(void) const; 
    void resize(size_t s);
    void print();
};

template <typename Node_T, typename Val_T>
AVLHashTable<Node_T,Val_T>::AVLHashTable():table(MINSIZE) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<MINSIZE; ++i) {
    table[i] = AVL<Node_T, Val_T>();
  }
}

template <typename Node_T, typename Val_T>
AVLHashTable<Node_T,Val_T>::AVLHashTable(size_t s):table(s) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<s; ++i) {
    table[i] = AVL<Node_T, Val_T>();
  }
}

template <typename Node_T, typename Val_T>
AVLHashTable<Node_T,Val_T>::~AVLHashTable() {
  for (size_t i=0; i<table.len(); ++i) {
    if (!table[i].isempty()) {
      PANIC("Hashtable not empty before destruction");
    }
    // explicitly destruct since array doesn't do that
    table[i].~AVL();
  }
};

template <typename Node_T, typename Val_T>
bool AVLHashTable<Node_T, Val_T>::insert(Node_T *new_node) {
  check_sizeup();
  // set the hash table size 
  new_node->hs = table.len();
  // Hash it
  size_t i = Node_T::hash(new_node->val()) % table.len();
  if (table[i].insert(new_node)) {
    count++;
    return true;
  }
  return false;
}

template <typename Node_T, typename Val_T>
Node_T* AVLHashTable<Node_T,Val_T>::get(Val_T key) {
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
Node_T* AVLHashTable<Node_T,Val_T>::remove(Node_T *n) {
  Val_T v = n->val();
  size_t i = Node_T::hash(v) % table.len();
  table[i].remove(n);
  count--;
  check_sizedown();
  return n;
}

template <typename Node_T, typename Val_T>
bool AVLHashTable<Node_T,Val_T>::isempty(void) const {
  return count == 0;
}

// TODO the runtime of this is still abysmal
// as it walks all the elements for every element it moves
template <typename Node_T, typename Val_T>
void AVLHashTable<Node_T,Val_T>::resize(size_t s) {
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
      table[i] = AVL<Node_T, Val_T>(); 
    }
  }
  // Rehash everything based on "s"
  for (size_t i=0; i<table.len(); ++i) {
    auto n = table[i].begin();
    while (n != table[i].end()) {
      if (n->hs != s) {
        auto node = &(*n);
        table[i].remove(node);
        size_t new_index = Node_T::hash(node->val()) % s;
        node->hs = s;
        table[new_index].insert(node);
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
void AVLHashTable<Node_T,Val_T>::check_sizedown(void) {
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
void AVLHashTable<Node_T,Val_T>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table.len() < 2*count) {
    resize(table.len()*2); 
  } 
}
template <typename Node_T, typename Val_T>
void AVLHashTable<Node_T,Val_T>::print(void) {
  printf("[\n");
  for (size_t i=0; i<table.len(); ++i) {
    printf("  ");
    table[i].print();
  }
  printf("]\n");
}
 

#endif