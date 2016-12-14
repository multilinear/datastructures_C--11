/*
 * Copyright 2014-05-13: Matthew Brewer
 *
 * Basically, don't use this... ever.
 * Most operations are linear time.
 *
 * This dictionary is intended to be the simplest possible dictionary. The point
 * is to give us something to compare behavior to for our faster more efficient
 * dictionaries. So, it's for use in small'ish unittests *only*.
 *
 * For large unittests, use this one to test a simpler implementation that doesn't
 * require as large of tests (like AVL). Then use that to test more complex code
 * like a BTree, where you need many nodes to hit some of the edge-cases.
 *
 * WARNING! This implements a multiset style dict! Multiple entries can be added
 * The first one found is the first returned/removed/etc. This differs from most
 * of the dictionary implementations in this library!
 *
 * !!!!Please, please, don't ever use this in production code!!!!
 *
 */

#include <cstring>
#include "panic.h"

#ifndef TRIVIALDICT_H
#define TRIVIALDICT_H

template<typename T, typename Val_T>
class TrivialDict {
  private:
    T *ar; 
    size_t used;
    size_t size;
  public:
    TrivialDict(size_t insize) { 
      size = insize;
      ar = new T[size];
      used = 0;
    }
    ~TrivialDict() {
      delete ar;
    }
    void reset(size_t insize) {
      if (size != insize) {
        delete ar;
        size = insize;
        ar = new T[size];
      }
      used = 0;
    }
    bool isempty() {
      return used==0;
    }
    bool insert(T el) {
      if (used == size) {
        printf("TrivialDict, used=%ld, size=%ld\n", used, size);
        PANIC("TrivialDict overflow!");
      }
      ar[used++] = el;
      return true;
    }
    T* get(Val_T k) {
      size_t i;
      for (i=0; i<used; i++) {
        if (T::compare(ar[i].val(),k) == 0) {
          return &(ar[i]);
        }
      }
      return nullptr;
    }
    bool remove(Val_T k) {
      size_t i;
      for (i=0; i<used; i++) {
        if (T::compare(ar[i].val(),k) == 0) {
          break;
        }
      }
      if (i == used) {
        return false;
      }
      // if used=5 and i=1, there are 3 elements to move so used-i-1
      memmove(&ar[i], &ar[i+1], (used-i-1) * sizeof(T));
      used--;
      return true;
    }
  class Iterator {
    private:
      size_t index; 
      TrivialDict<T,Val_T> *dict;   
    public:
      Iterator(TrivialDict<T,Val_T> *indict, size_t i) {
        index = i;
        dict = indict; 
      }
      Iterator(const TrivialDict<T,Val_T>::Iterator& iterator) {
        index = iterator.index;
        dict = iterator.dict;
      }
      ~Iterator() {}

      Iterator& operator=(const Iterator& other) {
        index = other.index;
        dict = other.dict;
        return(*this);
      }

      bool operator==(const Iterator& other) {
        return dict == other.dict && index == other.index;
      }

      bool operator!=(const Iterator& other) {
         return dict != other.dict || index != other.index;
      }

      Iterator& operator++() {
         index++;
         return *this;
      }

      Iterator operator++(int) {
         Iterator tmp(*this);
         ++(*this);
         return tmp;
      }

      T& operator*() {
         return dict->ar[index];
      }

      T* operator->() {
         return(&*(TrivialDict<T,Val_T>::Iterator)*this);
      }
   };
   Iterator begin() {
     return Iterator(this, 0);
   }
   Iterator end() {
     return Iterator(this, used);
   }
  void print() {
    auto i = begin();
    printf("[");
    for (;i != end(); i++) {
      if (i != begin()) {
        printf(",");
      }
      i->print();
    }
    printf("]");
  }
};

#endif
