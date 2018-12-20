/*
 * Copywrite: Matthew Brewer 2014-04-27 (Actually written by me  much earlier)
 */

#include "panic.h"
#include <algorithm>

#ifndef ARRAY_H
#define ARRAY_H

/*
These are various length-encoding array implementations.

Goal:
The goal of these implementations is to acheive the
the same performance as using a direct C array.

Design Choices:
We do not use an abstract class and inheritence to make these fully
interoperable as that would require virtual inheritence and thus
virtual dispatch. Instead, code that uses Arrays should be templated
on the specific array type.

Swap is here so that "sort" doesn't have to care what type it's working on
only what array type, this makes practical usage of sorts less cluttered.

We do not construct or delete objects in the array... This keeps allocation
and deletion at constant time (or malloc() time anyway), rather than linear.

All Array implementations share a similar meaning for:
- Constructor()
- Constructor(T ar[], size_t len)
- get(size_t)
- operator[](size_t)
- revi(size_t)
- size() 
- swap(size_t, size_t)

All Array implementations but StaticArray also share
- resize(size_t)

All UArray (Standing for Used Array) implementations further share
- size_t size()
- bool push(T*)
- bool pop(T*)
- bool isfull()
- bool isempty()
- bool operator bool()

(len for these means "length of the active portion of the array")

Code that templatizes on Array type can take advantage of these

Threadsafety:
  Thread compatible
*/

// Define "ARRAY_DEBUG" to enable bounds-checking
// If enabled this makes these arrays far more expensive than C-style
#ifdef ARRAY_DEBUG
#define ARRAY_CHECK(i) check(i)
#else
#define ARRAY_CHECK(i)
#endif

// Generalized function for copying one array into another
// Writing it this way means we don't need to write a constructor for
// *every* pair of array types
template<typename AT1, typename AT2>
void array_copy(AT1 *dest, const AT2 *src) {
  if (dest->size() != src->size()) {
    dest->resize(src->size());
  }
  for (size_t i = 0; i<src->size(); i++) {
    (*dest)[i] = (*src)[i];
  }
}

// This is a simple static array including no dynamic allocation
template<typename T, size_t Size>
class StaticArray {
  template<typename AT1, typename AT2>
  friend void array_copy(AT1 *dest, const AT2 *src);
  private:
    T ar[Size];
  public:
    typedef T value_type;
    StaticArray() {
    }
    StaticArray(T input[], size_t input_l) {
      if (input_l > Size) {
        PANIC("Static Array initializer is too long");
      }
      std::copy<T*, T*>(input, input+input_l, ar);
    }
    StaticArray(StaticArray<T, Size>* input) {
      array_copy<StaticArray<T, Size>, StaticArray<T, Size>>(this, input);
    }
    const T get(size_t index) const {
      ARRAY_CHECK(index);
      return ar[index];
    }
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(Size-index);
      return ar[Size-index];
    }
    size_t size() const {
      return Size;
    }
    void swap(size_t i, size_t j) {
      ARRAY_CHECK(i);
      ARRAY_CHECK(j);
      T tmp = ar[i];
      ar[i] = ar[j];
      ar[j] = tmp;
    }
    // ** check
    void check(size_t index) const {
      if (index >= Size) {
        printf("StaticArray %p access out of bounds index:%lu >= size:%lu\n", this, index, Size);
        PANIC("StaticArray access out of bounds index");
      }
    }
    bool isempty() const {
      return !Size;
    }
    operator bool() const {
      return Size;
    }
};

// This is a static array including no dynamic allocation, but tracking usage
// "size()" returns the actively used portion of the array, not the
// Total available size
// This is useful for writing datastructures, for stack allocation etc.
// Due to being statically allocated this should have no overhead compared
// To using an inline C-style array that tracks how much of the array is used.
template<typename T, size_t Size>
class StaticUArray {
  template<typename AT1, typename AT2>
  friend void array_copy(AT1 *dest, const AT2 *src);
  private:
    StaticArray<T,Size> ar;
    size_t _used;
  public:
    typedef T value_type;
    StaticUArray() {
      _used = 0;
    }
    StaticUArray(T input[], size_t input_l):ar(input, input_l) {
      _used = input_l;
    }
    StaticUArray(StaticUArray<T, Size>* input) {
      array_copy<StaticUArray<T, Size>, StaticUArray<T, Size>>(this, input);
    }
    bool push(T data) {
      if (_used == Size) {
        return false;
      }
      ar[_used++] = data;
      return true;
    }
    bool pop(T *val) {
      if (_used > 0) {
        _used--;
        *val = ar[_used];
        return true;
      }
      return false;
    }
    void drop() {
      if (_used > 0) {
        _used--;
      }
    }
    void resize(size_t size) {
      if (size > Size) {
        PANIC("Size is larger than static array size");
      }
      _used = size;
    }
    const T& get(size_t index) const {
      ARRAY_CHECK(index);
      return ar->get(index);
    }
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(_used-index);
      return ar[_used-index];
    }
    size_t size() const {
      return _used;
    }
    size_t capacity() const {
      return Size;
    }
    bool isfull() const {
      return _used >= Size;
    }
    bool isempty() const {
      return !_used;
    }
    operator bool() const {
      return _used;
    }
    void swap(size_t i, size_t j) {
      ARRAY_CHECK(i);
      ARRAY_CHECK(j);
      T tmp = ar[i];
      ar[i] = ar[j];
      ar[j] = tmp;
    }
    // ** check
    void check(size_t index) const {
      ar.check(index);
      if (index >= _used) {
        printf("StaticUArray %p access out of bounds index:%lu >= used:%lu\n", this, index, _used);
        PANIC("StaticUArray access of uninitialized data\n");
      }
    }
};

#endif
