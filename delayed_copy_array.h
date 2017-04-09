/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 
 * (this header added 2017-02-04)
 *
 * This is a "delayed copy" doubling array implementation.
 * This is for use as a stack *only*.
 * By amortizing the copy to a new array when the array is resized
 * we avoid the worst-case linear operation time, making resize
 * take only the time to malloc (plus constant) and all other operations
 * constant time.
 * To make this guarantee it assumes you will only use it like a stack, it
 * should work even if you don't but accesses outside the stack usage pattern
 * may incur a linear time copy cost.
 *
 * When to use this:
 * This is the preferred implementation for a stack, and also the preferred
 * backing store for a heap (unless using an external store). Otherwise
 * you probably don't want to.
 *
 * Performance notes:
 * In benchmark tests we found:
 * ~30% time overhead over a normal doubling array on small heaps
 * ~15% time overhead over a normal doubling array on large heaps
 * Theoretical memory costs are:
 * ~1/2 max memory usage overhead of a normal doubling array
 */


#include "panic.h"
#include "array.h"

#ifndef DELAYED_COPY_ARRAY_H
#define DELAYED_COPY_ARRAY_H

#ifdef DELAYED_COPY_ARRAY_DEBUG
#define DELAYED_COPY_ARRAY_CHECK(i) check(i)
#else
#define DELAYED_COPY_ARRAY_CHECK(i)
#endif

// This is an array that's designed to change in size a lot
// This is for use in heaps and stacks and that sort of thing
// It's a doubling array, including memory reclamation on downsizing
// "len()" returns the actively used portion of the array, not the
// Total available size
template<typename T>
class DCUArray {
  private:
    const size_t MINSIZE=4;
    T* ar1 = nullptr;
    T* ar2 = nullptr;
    size_t l1 = 0;
    size_t used = 0;
    size_t to_copy = 0;

    void resize_down_helper(size_t size) {
      // Only resize down so far, so we don't keep allocing and freeing when
      // nearly empty
      if (size < MINSIZE) {
        size = MINSIZE;
      }
      if (size == l1) {
        return;
      }
      // We assume realloc down is always in place
      // and thus this won't incur a linear copy
      // this is true in most implementations... this isn't
      // needed for correctness, but it is needed for time bounds
      ar1 = (T*) realloc(ar1, size * sizeof(T));
      l1 = size; 
      if (to_copy > size) {
        to_copy = size;
      }
      return;
    }

    void resize_up_helper(size_t size) {
      // Copy the data we haven't yet
      for (size_t i=to_copy; i > 0; i--) {
        ar1[i-1] = ar2[i-1];
      }
      // Now we need to copy the whole old array over
      to_copy = used;
      // Lets not get in to constructors/destructors... those will
      // get called as copy constructors if a node gets used.
      // So we just want to use malloc/free here.
      // We explicitly free and malloc (not realloc) because realloc will
      // copy the data over, and the whole point of this is to avoid that 
      // linear time copy
      if (ar2 != nullptr) {
        free(ar2);
      }
      ar2 = ar1;
      ar1 = (T*) malloc(size * sizeof(T));
      l1 = size;
    }
    
    void inc(){
      if (to_copy == 0) {
        return;
      }
      ar1[to_copy-1] = ar2[to_copy-1];
      to_copy = to_copy-1;
      // Free the old one if we finished copying
      if (to_copy == 0 && ar2 != nullptr) {
        free(ar2);
        ar2 = nullptr;
      }
    }

  public:
    DCUArray() {}

    DCUArray(size_t size) {
      this->resize(size);
    }

    DCUArray(T input[], size_t input_l) {
      this->resize(input_l);
      for (size_t i=0; i<input_l; i++) {
        (*this)[i] = input[i];
      }
    }

    DCUArray(DCUArray<T>* input) {
      array_copy<DCUArray<T>, DCUArray<T>>(this, input);
    }

    ~DCUArray() {
      if (ar1 != nullptr) {
        free(ar1);
        ar1 = nullptr;
        l1 = 0;
      }
      if (ar2 != nullptr) {
        free(ar2);
        ar2 = nullptr;
      }
    }

    void push(T data) {
      inc();
      if (used == l1) {
        // We start at MINSIZE to save time, we should never realloc for
        // repeated add-remove of one element (as this is quite common)
        resize_up_helper(l1 == 0 ? MINSIZE : l1 * 2);
      }
      ar1[used] = data;
      used++;
    }

    bool pop(T *val) {
      inc();
      if (used == 0) {
        return false;
      }
      used--;
      // Because we just ran "inc" we know to_copy < used
      *val = ar1[used];
      // Reclaim memory if the amount used gets small
      // we use /3 so adding and removing one item won't cause it to flutter
      if (used <= l1/3) {
        resize_down_helper(used);
      }
      return true;
    }

    void resize(size_t size) {
      if (size > l1) {
        resize_up_helper(size);
        used = size;
        return;
      }
      if (size <= l1/3) {
        resize_down_helper(size);
        used = size;
        return;
      }
      used = size;
    }

    void drop() {
      if (used > 0) {
        used--;
        // Reclaim memory if the amount used gets small
        // we use /3 so adding and removing one item won't cause it to flutter
        if (used <= l1/3) {
          resize_down_helper(used);
        }
      }
    }
    const T& get(size_t index) const {
      ARRAY_CHECK(index);
      if (index < to_copy) {
        return ar2[index];
      }
      return ar1[index];
    }
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      if (index < to_copy) {
        return ar2[index];
      }
      return ar1[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(used-index);
      return (*this)[used-index];
    }
    size_t len() const {
      return used;
    }
    bool isfull() const {
      return false;
    }
    bool isempty() const {
      return !used;
    }
    operator bool() const {
      return used;
    }
    void swap(size_t i, size_t j) {
      ARRAY_CHECK(i);
      ARRAY_CHECK(j);
      T tmp = get(i);
      (*this)[i] = get(j);
      (*this)[j] = tmp;
    }
    void check(size_t index) const {
      if (used > l1) {
        PANIC("Array, more elements used than exist\n");
      }
      if (index >= l1) {
        PANIC("Array access out of bounds\n");
      }
      if (index >= used) {
        PANIC("Array access of uninitialized data\n");
      }
    }
};

#endif
