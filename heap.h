/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 
 * (this header added 2017-04-27)
 * 
 * This is a simple heap implementation
 *
 * When to use this:
 * Any time you want a heap! (a priority queue)
 * 
 * How to use this:
 * We suggest using a DCUArray from delayed_copy_array.h for the backing store
 * (first argument). This will give a log(n) bound on all operations for a small
 * overhead. If you are okay with a linear bound for a little better average case
 * use UArray from array.h instead.
 */

#include <stdio.h>
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
 
template<typename UArrayT, typename T, typename C>
class Heap {
  private:
    UArrayT ar;
    void _check(size_t i) {
      int c;
      if (2*i+1 < ar.size()) {
        c = C::compare(&ar[2*i+1], &ar[i]);
        if (c < 0) {
          printf("2*i+1=%lu i=%lu\n", 2*i+1, i);
          PANIC("heap is not in order\n");
        }
        _check(2*i+1);
      }
      if (2*i+2 < ar.size()) {
        c = C::compare(&ar[2*i+2], &ar[i]);
        if (c < 0) {
          PANIC("heap is not in order\n");
        }
        _check(2*i+2);
      }
    }
    void bubble_down() {
      size_t i = 0;
      size_t j;
      while(true) {
        size_t left = 2*i + 1;
        size_t right = 2*i + 2;
        if (right < ar.size()) {
          // find the smaller value
          int c = C::compare(&ar[left], &ar[right]);
          if (c > 0) {
            j = right;
          } else {
            j = left; 
          }
        } else if (left < ar.size()) {
          j = left;
        } else {
          return;
        }
        // and if i is larger than j we need to swap
        // if not we're done
        int c = C::compare(&ar[i], &ar[j]);
        if (c > 0) {
          ar.swap(i,j);
        } else {
          return;
        }
        // go to where we moved the data and try again
        i = j;
      }
    }
    void bubble_up() {
      size_t i = ar.size()-1;
      size_t parent;
      while(i != 0) {
        parent = (i-1)/2;
        int c = C::compare(&ar[parent], &ar[i]);
        if (c>0) {
          ar.swap(parent, i);
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
      ar.push(data);
      bubble_up();
      HEAP_CHECK();
    }
    bool pop(T *val) {
      HEAP_CHECK();
      if (ar.size() > 1) {
        *val = ar[0];
        // notionally we'd like to do this... but
        // pop start mutation before it writes, making
        // the reference potentially no longer valid
        //ar.pop(&(ar[0]));
        // So we do this instead
        // TODO cast of tmp to an rvalue would use move semantics, saving a copy
        T tmp;
        // We shouldn't need this check, but otherwise we get an uninitialized variable error
        if (!ar.pop(&tmp)){
          PANIC("This should never happen");
        }
        ar[0] = tmp;
        bubble_down();
        //HEAP_CHECK();
        return true;
      }
      HEAP_CHECK();
      return ar.pop(val);
    }
    bool isempty() const {
      return !ar;
    }
    operator bool() const {
      return ar;
    }
    void check() {
      _check(0);
    }
    size_t size() const {
      return ar.size();
    }
    const T& get(size_t i) {
      return ar.get(i);
    }
};

#endif
