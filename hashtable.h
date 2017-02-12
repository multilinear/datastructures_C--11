#include "panic.h"
#include "array.h"
#include "dlist.h"

#ifndef HASHTABLE_H
#define HASHTABLE_H

// TODO: hashtable currently compares only hash, should compare actual values
// otherwise correctness is not guaranteed given hash collisions

#define MINSIZE 16

template <typename T>
class HashTableNode: public DListNode_base<HashTableNode<T>> {
  public:
    T datum;
    // The size when this was hashed, used like a generation counter
    // So we can track what was rehashed during a resize
    size_t hs;
    HashTableNode(T &d, size_t s) {
      datum = d;
      hs = s;
    }
};

template <typename T, typename H>
class HashTable {
  private:
    Array<DList<HashTableNode<T>>> table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
    HashTable();
    HashTable(size_t s);
    ~HashTable();
    bool insert(T datum);
    T* get(T key);
    bool remove(T key);
    bool isempty(void) const; 
    void resize(size_t s);
};

template <typename T, typename H>
HashTable<T,H>::HashTable():table(MINSIZE) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<MINSIZE; ++i) {
    table[i] = DList<HashTableNode<T>>();
  }
}

template <typename T, typename H>
HashTable<T,H>::HashTable(size_t s):table(s) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<s; ++i) {
    table[i] = DList<HashTableNode<T>>();
  }
}

template <typename T, typename H>
HashTable<T,H>::~HashTable() {
  for (size_t i=0; i<table.len(); ++i) {
    auto n = table[i].begin();
    while (n != table[i].end()) {
      table[i].remove(&(*n));
      count--;
      delete &(*n);
      n = table[i].begin();
    }
    // explicitly destruct since array doesn't do that
    table[i].~DList();
  }
};

template <typename T, typename H>
bool HashTable<T,H>::insert(T datum) {
  check_sizeup();
  size_t h = H::hash(datum);
  size_t i = h % table.len();
  // This is just to check for repeat keys
  // if we allow repeat keys we can remove this
  // TODO: maybe everything should have a "contains" operator?
  for (auto n = table[i].begin(); n != table[i].end(); ++n) {
    if (H::hash(n->datum) == h) {
      return false;
    }
  } 
  table[i].enqueue(new HashTableNode<T>(datum, table.len()));
  count++;
  return true;
}

template <typename T, typename H>
T* HashTable<T,H>::get(T key) {
  size_t h = H::hash(key);
  size_t i = h % table.len();
  for (auto n = table[i].begin(); n != table[i].end(); ++n) {
    if (H::hash(n->datum) == h) {
      return &(n->datum);
    }
  }
  return nullptr;
}

template <typename T, typename H>
bool HashTable<T,H>::remove(T key) {
  size_t h = H::hash(key);
  size_t i = h % table.len();
  // We don't use "dlist.remove" because this requires key
  // be a reference to the actual node, we want to find it by hash
  for (auto n = table[i].begin(); n != table[i].end(); ++n) {
    if (H::hash(n->datum) == h) {
      table[i].remove(&(*n)); 
      delete &(*n);
      count--;
      check_sizedown();
      return true;
    }
  }
  return false; 
}

template <typename T, typename H>
bool HashTable<T,H>::isempty(void) const {
  return count == 0;
}

template <typename T, typename H>
void HashTable<T,H>::resize(size_t s) {
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
      table[i] = DList<HashTableNode<T>>(); 
    }
  }
  // Rehash everything based on "s"
  for (size_t i=0; i<table.len(); ++i) {
    auto n = table[i].begin();
    while (n != table[i].end() && n->hs != s) {
      auto node = &(*n);
      table[i].remove(node);
      size_t new_index = H::hash(node->datum) % s;
      node->hs = s;
      table[new_index].enqueue(node);
      n = table[i].begin();
    }
  }
  // If decreasing we resize after
  if (s < table.len()) {
    table.resize(s);
  }
}

template <typename T, typename H>
void HashTable<T,H>::check_sizedown(void) {
  // If it's under a quarter full resize down
  if (table.len() > 4*count) {
    size_t s = table.len() / 2;
    if (s < MINSIZE) {
      s = MINSIZE;
    }
    resize(s); 
  }
}

template <typename T, typename H>
void HashTable<T,H>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table.len() < 2*count) {
    resize(table.len()*2); 
  } 
}

#endif
