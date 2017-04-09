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
 * API:
 * Note that this has the same API as Array (in array.h) not UArray
 * This algorithm is for random access, if you want something for stack-like
 * usage, see DCUARRAY in delayed_copy_array.h
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
#include "array.h"

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
class ZeroArray {
  private:
    struct Data {
      T data;
      size_t index; 
      Data(size_t i, const T& d) {
        index = i;
        data = d;
      }
    };
    Data* ar;
    size_t used;
    size_t* index;
    size_t length;
    T z;
    bool is_valid(size_t i) const {
      return (index[i] < used) && (ar[index[i]].index == i);
    }
  public:
    ZeroArray() {
      ar = nullptr;
      index = nullptr;
      length = 0;
      used = 0;
    }
    ZeroArray(T &z, size_t size) {
      ar = (Data*) malloc(size*sizeof(Data));
      index = (size_t*) malloc(size*sizeof(size_t));
      length = size;
      used = 0;
    }
    ZeroArray(T input[], size_t size) {
      ar = (Data*) malloc(size*sizeof(Data));
      index = (size_t*) malloc(size*sizeof(size_t));
      length = size;
      used = 0;
      for (size_t i = 0; i<size; i++) {
        set(i, input[i]);
      }
    }
    ZeroArray(ZeroArray<T>* input) {
      ar = (Data*) malloc(input.len()*sizeof(Data));
      index = (size_t*) malloc(input.len()*sizeof(size_t));
      length = input.len();
      used = 0;
      array_copy<ZeroArray<T>, ZeroArray<T>>(this, input);
    }
    ~ZeroArray() {
      if (ar) {
        free(ar);
        free(index);
      }
    }
    void zero(T _z) {
      used = 0;
      z = _z;
    }
    void set_zero(T _z) {
      z = _z;
    }
    void reset(const size_t new_size) {
      if (ar) {
        free(ar);
        free(index);
      }
      ar = (Data*) malloc(new_size*sizeof(Data));
      index = (size_t*) malloc(new_size*sizeof(size_t));
      length = new_size;
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
      ZERO_ARRAY_CHECK(i);
      ZERO_ARRAY_CHECK(j);
      T tmp = get(i);
      set(i, get(j));
      set(j, tmp);
    }
    const T& get(size_t i) const{
      ZERO_ARRAY_CHECK(i);
      if (!is_valid(i)) {
        return z;
      }
      return ar[index[i]].data;
    }
    void set(const size_t i, const T& d) {
      ZERO_ARRAY_CHECK(i);
      if (is_valid(i)) {
        ar[index[i]].data = d;
        return;
      }
      ar[used] = Data(i, d);
      index[i] = used;
      used++;
    }
    const T& getDefault(size_t i, const T& d) const{
      ZERO_ARRAY_CHECK(i);
      if (!is_valid(i)) {
        set(i, d);
      }
      return ar[index[i]].data;
    }
    T& operator[](size_t i) {
      ZERO_ARRAY_CHECK(i);
      if (!is_valid(i)) {
        set(i, z);
      }
      return ar[index[i]].data;
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t i) {
      ZERO_ARRAY_CHECK(length-i);
      return (*this)[length-i];
    }
    // ** Check
    void check(size_t i) const {
      if (i >= length) {
        printf("\nPANIC: index=%lu length=%lu\n", i, length);
        PANIC("ZeroArray access out of bounds\n");
      }
    }
};

#endif
