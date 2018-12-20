/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A standard open chaining hashtable implementation, using external allocation
 * for nodes.
 *
 * When to use this:
 *   If you want a dictionary, want the fastest average case, and don't care
 *   about worst case. avlhashtable.h seems to be equivelent in performance.
 *
 * Note: This could be generalized to use any dict-type structure, but during
 * resize we take advantage of the dlist's queue ordering properties... if you
 * want to make one general take a look at avlhashtable
 *
 * resizes up when size is < x data it contains
 * resizes down when size is > 2x data it contains
 *
 * Faster average case than btree by ~2x
 *
 * Worst case operation is linear per op due to
 * 1) linear rehash
 * 2) possability of every item hash colliding
 *
 * Threadsafety:
 *   thread compatible
 */ 

#include "panic.h"
#include "array.h"
#include "dlist.h"
#include <vector>

#ifndef OC_HASHTABLE_H
#define OC_HASHTABLE_H

#define MINSIZE 4

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
    std::vector<DList<Node_T, Val_T>> table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
    class Iterator {
      private:
        std::vector<DList<Node_T,Val_T>> *table;
        size_t index;
        typename DList<Node_T,Val_T>::Iterator iter;
      public:
        Iterator(std::vector<DList<Node_T,Val_T>> *t, size_t ind) {
          table = t;
          index = ind;
          iter = (*table)[index].begin();
          // Look for a valid element (if we don't have one)
          while (index < table->size() && iter == (*table)[index].end()) {
            index++;
            iter = (*table)[index].begin();
          }
        }
        Iterator(const Iterator& other) {
          table = other.table;
          index = other.index;
          iter = other.iter;
        }
        Iterator& operator=(const Iterator& other) {
          index = other.index;
          iter = other.iter;
          return *this;
        }
        bool operator==(const Iterator& other) {
          return (index >= table->size() && other.index >= other.table->size()) ||
            index == other.index && iter == other.iter;
        }
        bool operator!=(const Iterator& other) {
          return !(index >= table->size() && other.index >= other.table->size()) && 
            (index != other.index || iter != other.iter);
        }
        Iterator operator++() {
          // If we're at the end, we're done
          if (index >= table->size()) {
            return *this;
          }
          // We were at a valid element (or the end of the array)
          // so iter++ makes sense
          iter++;
          while (iter == (*table)[index].end() && index < table->size()) {
            index++;
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
    OCHashTable();
    OCHashTable(size_t s);
    ~OCHashTable();
    bool insert(Node_T *n);
    Node_T* get(Val_T key);
    void remove(Node_T *n);
    bool isempty(void) const; 
    void resize(size_t s);
    void print();
};

template <typename Node_T, typename Val_T>
OCHashTable<Node_T,Val_T>::OCHashTable():table(MINSIZE) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<MINSIZE; ++i) {
    table.emplace_back(DList<Node_T,Val_T>());
  }
}

template <typename Node_T, typename Val_T>
OCHashTable<Node_T,Val_T>::OCHashTable(size_t s):table(s) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<s; ++i) {
    table[i] = DList<Node_T,Val_T>();
  }
}

template <typename Node_T, typename Val_T>
OCHashTable<Node_T,Val_T>::~OCHashTable() {
  for (size_t i=0; i<table.size(); ++i) {
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
  new_node->hs = table.size();
  size_t i = Node_T::hash(v) % table.size();
  if (table[i].get(v)) {
    return false;
  }
  table[i].enqueue(new_node);
  count++;
  return true;
}

template <typename Node_T, typename Val_T>
Node_T* OCHashTable<Node_T,Val_T>::get(Val_T key) {
  size_t i = Node_T::hash(key) % table.size();
  auto n = table[i].get(key);
  if (n) {
    return &(*n);
  }
  return nullptr;
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::remove(Node_T *n) {
  // Note, if n is not in the hashtable, this will cause
  // some nasty corruption.
  Val_T v = n->val();
  size_t i = Node_T::hash(v) % table.size();
  table[i].remove(&(*n)); 
  count--;
  check_sizedown();
}

template <typename Node_T, typename Val_T>
bool OCHashTable<Node_T,Val_T>::isempty(void) const {
  return count == 0;
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::resize(size_t s) {
  // nothing to do
  if (s == table.size()) {
    return;
  }
  // If increasing size, we resize before we rehash
  if (s > table.size()) {
    table.resize(s);
  }
  // Rehash everything based on "s"
  for (size_t i=0; i<table.size(); ++i) {
    // because dlist is a *queue* we can cheat!
    // everything we've rehashed will be on one end
    // so we can just peak 'til we hit one!
    while (table[i].peak() && table[i].peak()->hs != s) {
      auto node = table[i].dequeue();
      size_t new_index = Node_T::hash(node->val()) % s;
      node->hs = s;
      // We remove and enqueue even if it didn't move
      // this saves us a linear search
      table[new_index].enqueue(node);
    }
  }
  // If decreasing we resize after
  if (s < table.size()) {
    table.resize(s);
  }
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::check_sizedown(void) {
  // If it's under half full resize down
  if (table.size() > 2*count) {
    size_t s = table.size() / 2;
    if (s < MINSIZE) {
      s = MINSIZE;
    }
    resize(s); 
  }
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::check_sizeup(void) {
  // If it's over full resize up
  if (table.size() < count) {
    resize(table.size()*2); 
  } 
}

template <typename Node_T, typename Val_T>
void OCHashTable<Node_T,Val_T>::print(void) {
  printf("[\n");
  for (size_t i=0; i<table.size(); ++i) {
    printf("  ");
    table[i].print();
  }
  printf("]\n");
}

#endif
