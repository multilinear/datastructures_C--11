/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A hashtable implementation, much like open chaining, but using a btree in place
 * of the standard linked-list.
 *
 * When to use this:
 * Don't, it's incredibly slow with no advantages. Use btree.h or avlhashtable,
 * or ochashtable.h instead
 *
 * This algorithm turned out to be a very bad idea, due to btree's poor
 * performance on small dictionaries. avlhashtable
 *
 * resizes up when size is < x data it contains
 * resizes down when size is > 2x data it contains
 *
 * Worst case operation is linear per op due to linear rehash
 * Next worst case is all elements hash collide, and operations are log(N)
 * 
 * Threadsafety:
 *   thread compatible
 */ 

#include "panic.h"
#include "btree.h"
#include "stdio.h"
#include <vector>

#ifndef BTREE_HASHTABLE_H
#define BTREE_HASHTABLE_H

#define MINSIZE 4
#define ARITY 64

template <typename Data_T, typename Val_T, typename HC>
class BTreeHashTable {
  private:
    std::vector<BTree<Data_T, Val_T, HC, ARITY>> *table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
   	class Iterator {
      private:
        std::vector<BTree<Data_T, Val_T, HC, ARITY>> *table;
        size_t index;
			  typename BTree<Data_T, Val_T, HC, ARITY>::Iterator iter;
      public:
        Iterator(std::vector<BTree<Data_T, Val_T, HC, ARITY>> *t, size_t ind) {
          table = t;
          index = ind;
          // Make sure iter is valid at least
          if (index < table->size()) {
            iter = (*table)[index].begin();
          }else  {
            iter = (*table)[index-1].end();
          }
          // Look for for an actual element (where iter != end)
          while (index < table->size() && iter == (*table)[index].end()) {
            index++;
            if (index < table->size()) {
              iter = (*table)[index].begin();
            }
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
          return (index >= table.size() && other.index >= other.table.size()) ||
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
          while (iter == (*table)[index].end() && index < (*table).size()) {
            index++;
            if (index < table->size()) {
              iter = (*table)[index].begin();
            }
          }
          return *this;
        }
        Iterator operator++(int) {
          Iterator tmp(*this);
          ++(*this);
          return tmp;
        }
        Data_T& operator*() {
					// Get what's inside the iterator
          return *iter;
        }
        Data_T* operator->() {
					// Get a reference to what's inside the iterator (lol)
          return &(iter);
        }
    };
    Iterator begin() {
      return Iterator(table, 0);
    }
    Iterator end() {
      return Iterator(table, table->size());
    }

    BTreeHashTable();
    BTreeHashTable(size_t s);
    ~BTreeHashTable();
    bool insert(const Data_T& data);
    Data_T* get(Val_T key);
    bool remove(Val_T key, Data_T* data);
    bool isempty(void) const; 
    void resize(size_t s);
    void print(void);
};

template <typename Data_T, typename Val_T, typename HC>
BTreeHashTable<Data_T, Val_T, HC>::BTreeHashTable() {
  table = new std::vector<BTree<Data_T, Val_T, HC, ARITY>>(MINSIZE);
}

template <typename Data_T, typename Val_T, typename HC>
BTreeHashTable<Data_T, Val_T, HC>::BTreeHashTable(size_t s) {
  table = new std::vector<BTree<Data_T, Val_T, HC, ARITY>>(s);
}

template <typename Data_T, typename Val_T, typename HC>
BTreeHashTable<Data_T, Val_T, HC>::~BTreeHashTable() {
  delete table;
};

template <typename Data_T, typename Val_T, typename HC>
bool BTreeHashTable<Data_T, Val_T, HC>::insert(const Data_T& data) {
  check_sizeup();
  Val_T v = HC::val(data);
  size_t i = HC::hash(v) % table->size();
  if ((*table)[i].insert(data)) {
    count++;
    return true;
  }
  return false;
}

template <typename Data_T, typename Val_T, typename HC>
Data_T* BTreeHashTable<Data_T, Val_T, HC>::get(Val_T key) {
  size_t i = HC::hash(key) % table->size();
  return (*table)[i].get(key);
}

template <typename Data_T, typename Val_T, typename HC>
bool BTreeHashTable<Data_T, Val_T, HC>::remove(Val_T v, Data_T *data) {
  size_t i = HC::hash(v) % table->size();
  bool b = (*table)[i].remove(v, data); 
  if (!b) {
    return false;
  }
  count--;
  check_sizedown();
  return true;
}

template <typename Data_T, typename Val_T, typename HC>
bool BTreeHashTable<Data_T, Val_T, HC>::isempty(void) const {
  return count == 0;
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::resize(size_t s) {
  // nothing to do
  if (s == table->size()) {
    return;
  }
  std::vector<BTree<Data_T, Val_T, HC, ARITY>> *new_table = new std::vector<BTree<Data_T, Val_T, HC, ARITY>>(s);

  // Rehash everything based on "s"
  size_t i = 0;
  while (i<table->size()) {
    // Find a node that needs to be rehashed
    auto n = (*table)[i].begin();
    if (n == (*table)[i].end()) {
      i++;
      continue;
    }
    Val_T v = *n;
    size_t new_index = HC::hash(v) % s;
    Data_T data;
    // remove it
    if (!(*table)[i].remove(v, &data)) {
      PANIC("Value wasn't in Tree!\n"); 
    }
    (*new_table)[new_index].insert(data);
  }
  delete table;
  table = new_table;
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::check_sizedown(void) {
  // If it's under a quarter full resize down
  if (table->size() > 2*count) {
    size_t s = table->size() / 2;
    if (s < MINSIZE) {
      s = MINSIZE;
    }
    resize(s); 
  }
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table->size() < count) {
    resize(table->size()*2); 
  } 
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::print(void) {
	printf("[\n");
  for (size_t i=0; i<table->size(); ++i) {
		printf("  ");
    (*table)[i].print();
  }
	printf("]\n");
}
#endif
