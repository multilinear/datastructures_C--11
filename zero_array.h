/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * Implementation of a somewhat esoteric algorithm.
 * This is a notional array, where we can "zero" (set to a common value)
 * every element in the array in constant time.
 * Normal set and get are still constant time, size is still linear.
 * 
 * This is not usually useful, BUT very occasionally you want to be able
 * to allocate a new array, but don't want the linear cost of zeroing the
 * whole thing.... e.g. you want to resize an array in constant time. 
 *
 * Algorithm:
 * Index array (used like a normal array)
 * Data array (used like a stack)
 * integer we'll call "used" which just says how much data is in the stack.
 * Basic idea:
 * Index tells us where in the Data array data is stored
 * If index points above "used" then we must not have initialized it 
 * When a "set" occurs on an unitialized index, we add it to the Data array
 * stack style, then point index at it.
 *
 * This *almost* works There is only one problem... "index[i]" might happen to
 * point below used, at data that really goes with a different index. 
 * To work around this the Data array also contains an index stating which index
 * each element is storing data for. So, in this case we notice
 * "Data[Index[i]].index != i" and thus i isn't really initialized yet.
 */

#include "panic.h"

#ifndef ZERO_ARRAY_H
#define ZERR_ARRAY_H

// Define "ARRAY_DEBUG" to enable bounds-checking
// If enabled this makes these arrays far more expensive than C-style
#ifdef ZERO_ARRAY_DEBUG
#define ZERO_ARRAY_CHECK(i) check(i)
#else
#define ZERO_ARRAY_CHECK(i)
#endif

// This is just a dynamically allocated array, as you are used to using one.
template<typename T>
class Array {
  private:
    struct Data {
      T data;
      size_t index; 
      Data(size_t i) {
        index = i;
      }
    }
    Data* ar;
    size_t used;
    size_t* index;
    size_t length;
    T zero;
    bool is_valid(size_t i) const {
      return (index[i] < used)  && ar[index[i]] == i);
    }
  public:
    Array(T &z) {
      z = zero;
      ar = nullptr;
      index = nullptr;
      length = 0;
    }
    Array(T &z, size_t size) {
      ar = malloc(size*sizeof(Data));
      index = malloc(size*sizeof(size_t));
      length = size;
    }
    Array(T input[], size_t size) {
      ar = malloc(size*sizeof(Data));
      index = malloc(size*sizeof(size_t));
      length = size;
      for (size_t i = 0; i<size; i++) {
        set(i, input[i]);
      }
    }
    Array(Array<T>* input) {
      ar = malloc(input.len()*sizeof(Data));
      index = malloc(input.len()*sizeof(size_t));
      length = input.len();
      array_copy<Array<T>, Array<T>>(this, input);
    }
    ~Array() {
      if (ar) {
        free(ar);
        free(index);
      }
    }
    void zero(T &z) {
      used = 0;
      zero = z;
    }
    void resize(size_t new_size) {
      if (ar) {
        free(ar);
        free(index);
      }
      ar = malloc(size*sizeof(Data));
      index = malloc(size*sizeof(size_t));
      length = size;
      used = 0;
    }
    size_t len() const {
      return length;
    }
    bool isempty() const {
      return length;
    }
    operator bool() const {
      return length;
    }
    void swap(size_t i, size_t j) {
      ARRAY_CHECK(i);
      ARRAY_CHECK(j);
      T tmp = get(i);
      set(i, get(j));
      set(j, tmp);
    }
    const T& get(size_t i) const{
      if (!is_valid()) {
        return zero;
      }
      return ar[index[i]].data;
    }
    void set(size_t i, T& d) {
      if (is_valid()) {
        ar[index[i]].data = d;
        return;
      }
      ar[used++] = Data(i);
      index[i] == ar.len()-1;
    }
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      if (!is_valid()) {
        set(i, zero);
      }
      return ar[index[i]].data;
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t i) {
      ARRAY_CHECK(_length-i);
      return (*this)[length-i];
    }
    // ** Check
    void check(size_t i) const {
      if (i >= length) {
        printf("\nPANIC: index=%lu length=%lu\n", i, length);
        PANIC("Array access out of bounds\n");
      }
    }
};

#endif
