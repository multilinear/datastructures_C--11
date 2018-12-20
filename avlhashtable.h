/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A standard open chaining hashtable implementation, using external allocation
 * for nodes.... EXCEPT that we use an avl tree instead of a linked-list
 *
 * resizes up when size is < x data it contains
 * resizes down when size is > 2x data it contains
 *
 * Worst case operation is linear per op due to linear rehash
 *
 * When to use this:
 * This is comparable with ochashtable, but taking a little more space
 * This is a reasonable algorithm to use if worst-case isn't your biggest concern
 *
 * Threadsafety:
 *   Thread compatible
 */ 

#include <stdio.h>
#include <utility>
#include "panic.h"
#include "delayed_copy_array.h"
#include "avl.h"

#ifndef AVL_HASHTABLE_H
#define AVL_HASHTABLE_H

#define MINSIZE 4

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
    DCUArray<AVL<Node_T, Val_T>> table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
    class Iterator {
      private:
        DCUArray<AVL<Node_T, Val_T>> *table;
        size_t index;
        typename AVL<Node_T, Val_T>::Iterator iter;
      public:
        Iterator(DCUArray<AVL<Node_T, Val_T>> *t, size_t ind) {
          table = t;
          index = ind;
          if (index >= table->size()) {
            iter = (*table)[0].end();
            return;
          } 
          iter = (*table)[index].begin();
          // Look for a valid element (if we don't have one)
          while (iter == (*table)[index].end()) {
            index++;
            if (index >= table->size()) {
              break;
            }
            iter = (*table)[index].begin();
          }
        }
        Iterator(const Iterator& other) {
          table = other.table;
          index = other.index;
          iter = other.iter;
        }
        Iterator& operator=(const Iterator& other) {
          table = other.table;
          index = other.index;
          iter = other.iter;
          return *this;
        }
        bool operator==(const Iterator& other) {
          return (index >= table->size() && other.index >= other.table->size()) ||
            (index == other.index && iter == other.iter);
        }
        bool operator!=(const Iterator& other) {
          return !((*this) == other);
        }
        Iterator operator++() {
          // If we're at the end, we're done
          if (index >= table->size()) {
            return *this;
          }
          // We were at a valid element (or the end of the array)
          // so iter++ makes sense
          iter++;
          while (iter == (*table)[index].end()) {
            index++;
            if (index >= (*table).size()) {
              iter = (*table)[0].end();
              break;
            } 
            iter = (*table)[index].begin();
          }
          return *this;
        }
        Iterator operator++(int) {
          Iterator tmp(*this);
          ++(*this);
          return tmp;
        }
        Node_T& operator*() {
					// Get what's inside the iterator
          return *iter;
        }
        Node_T* operator->() {
					// Get a reference to what's inside the iterator (lol)
          return &(*iter);
        }
    };
    Iterator begin() {
      return Iterator(&table, 0);
    }
    Iterator end() {
      return Iterator(&table, table.size());
    }
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
  for (size_t i=0; i<table.size(); ++i) {
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
  new_node->hs = table.size();
  // Hash it
  size_t i = Node_T::hash(new_node->val()) % table.size();
  if (table[i].insert(new_node)) {
    count++;
    return true;
  }
  return false;
}

template <typename Node_T, typename Val_T>
Node_T* AVLHashTable<Node_T,Val_T>::get(Val_T key) {
  size_t i = Node_T::hash(key) % table.size();
  return table[i].get(key);
}

template <typename Node_T, typename Val_T>
Node_T* AVLHashTable<Node_T,Val_T>::remove(Node_T *n) {
  Val_T v = n->val();
  size_t i = Node_T::hash(v) % table.size();
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
// If 3 elements stay, and 3 elements move, it'll walk the first
// 3 elements 3 times...
template <typename Node_T, typename Val_T>
void AVLHashTable<Node_T,Val_T>::resize(size_t s) {
  // nothing to do
  if (s == table.size()) {
    return;
  }
  // If increasing size, we resize before we rehash
  if (s > table.size()) {
    size_t old_size = table.size();
    table.resize(s);
    // We have to initialize the lists since
    // array doesn't construct objects it contains
    for (size_t i=old_size; i<s; ++i) {
      table[i] = AVL<Node_T, Val_T>(); 
    }
  }
  // Rehash everything based on "s"
  for (size_t i=0; i<table.size(); ++i) {
    auto n = table[i].begin();
    while (n != table[i].end()) {
      if (n->hs != s) {
        auto node = &(*n);
        size_t new_index = Node_T::hash(node->val()) % s;
        node->hs = s;
        if (new_index != i) {
          table[i].remove(node);
          table[new_index].insert(node);
          n = table[i].begin();
          continue;
        }
      } 
      n++;
    }
  }
  // If decreasing we resize after
  if (s < table.size()) {
    table.resize(s);
  }
}

template <typename Node_T, typename Val_T>
void AVLHashTable<Node_T,Val_T>::check_sizedown(void) {
  // If it's under a quarter full resize down
  if (table.size() > 2*count) {
    size_t s = table.size() / 2;
    if (s < MINSIZE) {
      s = MINSIZE;
    }
    resize(s); 
  }
}

template <typename Node_T, typename Val_T>
void AVLHashTable<Node_T,Val_T>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table.size() < count) {
    resize(table.size()*2); 
  } 
}
template <typename Node_T, typename Val_T>
void AVLHashTable<Node_T,Val_T>::print(void) {
  printf("[\n");
  for (size_t i=0; i<table.size(); ++i) {
    printf("  ");
    table[i].print();
  }
  printf("]\n");
}
 

#endif
