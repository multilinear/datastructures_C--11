#include <stdio.h>
#include "array.h"
#include "panic.h"

#ifndef HEAP_H
#define HEAP_H


// Define this to see tons of detail about what the tree is doing
#ifdef HEAP_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#else
#define PRINT(msg)
#endif

// Define this to implement some expensive consistancy checking, this is great
// for debugging code that uses the tree as well.
// This checks all of the AVL invariants before and after ever oparation.
#ifdef HEAP_DEBUG
#define HEAP_CHECK() check()
#else
#define HEAP_CHECK()
#endif
 
template<typename T, typename Val_T, typename C>
class Heap {
  private:
    UsedArray<T> ar;
    void _check(size_t i) {
      int c;
      if (2*i+1 < ar.used()) {
        c = C::compare(&ar[2*i+1], &ar[i]);
        if (c < 0) {
          PANIC("heap is not in order\n");
        }
        _check(2*i+1);
      }
      if (2*i+2 < ar.used()) {
        c = C::compare(&ar[2*i+2], &ar[i]);
        if (c < 0) {
          PANIC("heap is not in order\n");
        }
        _check(2*i+2);
      }
    }
    void swap(size_t index1, size_t index2) {
      T tmp = ar[index1];
      ar[index1] = ar[index2];
      ar[index2] = tmp;
    }
    void bubble_down() {
      size_t i = 0;
      size_t j;
      while(true) {
        size_t left = 2*i + 1;
        size_t right = 2*i + 2;
        if (right < ar.used()) {
          // find the smaller value
          int c = C::compare(&ar[left], &ar[right]);
          if (c > 0) {
            j = right;
          } else {
            j = left; 
          }
        } else if (left < ar.used()) {
          j = left;
        } else {
          return;
        }
        // and if i is larger than j we need to swap
        // if not we're done
        int c = C::compare(&ar[i], &ar[j]);
        if (c > 0) {
          swap(i,j);
        } else {
          return;
        }
        // go to where we moved the data and try again
        i = j;
      }
    }
    void bubble_up() {
      size_t i = ar.used()-1;
      size_t parent;
      while(i != 0) {
        parent = (i-1)/2;
        int c = C::compare(&ar[parent], &ar[i]);
        if (c>0) {
          swap(parent, i);
        } else {
          return;
        }
        i = parent;
      }
      return;
    }
  public:
    Heap():ar() {
    }
    ~Heap() {
    }
    void push(T data) {
      HEAP_CHECK();
      ar.append(data);
      bubble_up();
      HEAP_CHECK();
    }
    bool pop(T *val) {
      HEAP_CHECK();
      if (ar.used() > 1) {
        *val = ar[0];
        ar.pop(&ar[0]);
        bubble_down();
        HEAP_CHECK();
        return true;
      }
      HEAP_CHECK();
      return ar.pop(val);
    }
    void check() {
      _check(0);
    }
};

#endif
