/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A hashtable implementation, much like open chaining, but using a btree in place
 * of the standard linked-list.
 *
 * When to use this:
 *  Don't, ochashtable and avlhasthable are strictly better
 *
 * resizes up when size is < x data it contains
 * resizes down when size is > 2x data it contains
 * 
 * Worst case for all operations is linear
 *
 * Threadsafety:
 *   thread compatible
 */ 

#include "panic.h"
#include <vector>

#ifndef HASHTABLE_H
#define HASHTABLE_H

#define MINSIZE 4

template <typename Data_T, typename Val_T, typename HC>
class HashTable {
  private:
    std::vector<std::vector<Data_T>> *table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
   	class Iterator {
      private:
        std::vector<std::vector<Data_T>> *t;
        size_t i;
        size_t j;
      public:
        Iterator(std::vector<std::vector<Data_T>> *_t, size_t _i, size_t _j) {
          t = _t;
          i = _i;
          j = _j;
          // Look for a valid element (if we don't have one)
          while (i < t->size() && j >= (*t)[i].size()) {
            i++;
          }
        }
        Iterator(const Iterator& other) {
          t = other.t;
          i = other.i;
          j = other.j;
        }
        Iterator& operator=(const Iterator& other) {
          t = other.t;
          i = other.i;
          j = other.j;
          return *this;
        }
        bool operator==(const Iterator& other) {
          return (i >= t->size() && other.i >= other.t->size()) ||
            (i == other.i && j == other.j);
        }
        bool operator!=(const Iterator& other) {
          return !((*this) == other);
        }
        Iterator operator++() {
          // If we're at the end, we're done
          if (i >= t->size()) {
            return *this;
          }
          j++;
          while (i < t->size() && j >= (*t)[i].size()) {
            i++;
            j = 0;
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
          return (*t)[i][j];
        }
        Data_T* operator->() {
					// Get a reference to what's inside the iterator (lol)
          return &(*t)[i][j];
        }
    };
    Iterator begin() {
      return Iterator(table, 0, 0);
    }
    Iterator end() {
      return Iterator(table, table->size(), 0);
    }

    HashTable();
    HashTable(size_t s);
    ~HashTable();
    bool insert(const Data_T& data);
    Data_T* get(Val_T key);
    bool remove(Val_T key, Data_T* data);
    bool isempty(void) const; 
    void resize(size_t s);
    void print(void);
};

template <typename Data_T, typename Val_T, typename HC>
HashTable<Data_T, Val_T, HC>::HashTable() {
  table = new std::vector<std::vector<Data_T>>(MINSIZE);
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<MINSIZE; ++i) {
    (*table)[i] = std::vector<Data_T>();
  }
}

template <typename Data_T, typename Val_T, typename HC>
HashTable<Data_T, Val_T, HC>::HashTable(size_t s):table(s) {
  table = new std::vector<std::vector<Data_T>>(s);
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<s; ++i) {
    (*table)[i] = std::vector<Data_T>();
  }
}

template <typename Data_T, typename Val_T, typename HC>
HashTable<Data_T, Val_T, HC>::~HashTable() {
  delete table;
}

template <typename Data_T, typename Val_T, typename HC>
bool HashTable<Data_T, Val_T, HC>::insert(const Data_T& data) {
  check_sizeup();
  Val_T v = HC::val(data);
  size_t i = HC::hash(v) % table->size();
  // reject duplicates
  for (size_t j = 0; j < (*table)[i].size(); j++) {
    if (HC::val((*table)[i][j]) == v) {
      return false;
    }
  }
  (*table)[i].push_back(data);
  count++;
  return true;
}

template <typename Data_T, typename Val_T, typename HC>
Data_T* HashTable<Data_T, Val_T, HC>::get(Val_T key) {
  size_t i = HC::hash(key) % table->size();
  size_t j;
  for (j=0; j < (*table)[i].size(); j++) {
    if (HC::val((*table)[i][j]) == key) {
      return &((*table)[i][j]);
    }
  }
  return nullptr;
}

template <typename Data_T, typename Val_T, typename HC>
bool HashTable<Data_T, Val_T, HC>::remove(Val_T v, Data_T *data) {
  size_t i = HC::hash(v) % table->size();
  // Find it
  bool found = false;
  size_t j;
  for (j = 0; j < (*table)[i].size(); j++) {
    if (HC::val((*table)[i][j]) == v) {
      found = true;
      *data = (*table)[i][j];
      break;
    }
  }
  if (!found) {
    return false;
  }
  // Shift the table
  for (;j+1 < (*table)[i].size(); j++) {
    (*table)[i][j] = (*table)[i][j+1];
  }
  // Now that it's shifted, just drop the last element
  (*table)[i].pop_back();
  // bookkeeping
  count--;
  check_sizedown();
  return true;
 }

template <typename Data_T, typename Val_T, typename HC>
bool HashTable<Data_T, Val_T, HC>::isempty(void) const {
  return count == 0;
}

template <typename Data_T, typename Val_T, typename HC>
void HashTable<Data_T, Val_T, HC>::resize(size_t s) {
  // nothing to do
  if (s == table->size()) {
    return;
  }
  std::vector<std::vector<Data_T>> *old_table = table;
  table = new std::vector<std::vector<Data_T>>(s);
  // Initialize the new array
  for (size_t i=0; i<table->size(); i++) {
    (*table)[i] = std::vector<Data_T>();
  }
  // Rehash
  for (size_t i=0; i<old_table->size(); i++) {
    Data_T tmp;
    while ((*old_table)[i].size()) {
      tmp = (*old_table)[i].back();
      (*old_table)[i].pop_back();
      // We already swapped the tables, so normal insert should work fine
      // This way we don't have to duplicate our hashing logic
      Val_T v = HC::val(tmp);
      size_t index = HC::hash(v) % table->size();
      (*table)[index].push_back(tmp);
    }
  }
  delete old_table;
}

template <typename Data_T, typename Val_T, typename HC>
void HashTable<Data_T, Val_T, HC>::check_sizedown(void) {
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
void HashTable<Data_T, Val_T, HC>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table->size() < count) {
    resize(table->size()*2); 
  } 
}

template <typename Data_T, typename Val_T, typename HC>
void HashTable<Data_T, Val_T, HC>::print(void) {
	printf("[");
  for (size_t i=0; i<table->size(); ++i) {
		printf("[");
    for (size_t j=0; j<(*table)[i].size(); ++j) {
      HC::printV((*table)[i][j]);
      printf(",");
    }
		printf("]");
  }
	printf("]\n");
}
#endif
