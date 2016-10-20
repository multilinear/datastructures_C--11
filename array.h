#include "panic.h"

#ifdef ARRAY_DEBUG
#define ARRAY_CHECK(i) check(i)
#else
#define ARRAY_CHECK(i)
#endif

template<typename AT1, typename AT2>
void array_copy(AT1 *dest, AT2 *src) {
  if (dest->len() != src->len()) {
    dest->resize(src->len());
  }
  for (size_t i = 0; i<src->len(); i++) {
    (*dest)[i] = (*src)[i];
  }
}

template<typename T>
class Array {
  private:
    T *ar;
    size_t _length;
  public:
    Array() {
      ar = nullptr;
      resize(0);
    }
    Array(size_t size) {
      ar = nullptr;
      resize(size);
    }
    Array(T input[], size_t input_l) {
      ar = nullptr;
      resize(input_l);
      // TODO: This should use memcpy
      for (size_t i=0; i<input_l; i++) {
        ar[i] = input[i];
      }
    }
    Array(Array<T>* input) {
      ar = nullptr;
      _length = 0;
      array_copy<Array<T>, Array<T>>(this, input);
    }

    ~Array() {
      free(ar);
    }
    void resize(size_t new_size) {
      _length = new_size;
      ar = (T*) realloc(ar, _length * sizeof(T));
    }
    size_t len() {
      return _length;
    }
    void swap(size_t i, size_t j) {
      ARRAY_CHECK(i);
      ARRAY_CHECK(j);
      T tmp = ar[i];
      ar[i] = ar[j];
      ar[j] = tmp;
    }
    // ** Common functions
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(_length-index);
      return ar[_length-index];
    }
    // ** Check
    void check(size_t index) {
      if (index >= _length) {
        printf("\nPANIC: index=%lu length=%lu\n", index, _length);
        PANIC("Array access out of bounds\n");
      }
    }
};

template<typename T>
class DynArray {
  private:
    Array<T> ar;
    size_t _used;
  public:
    DynArray() {
      _used = 0;
    }
    DynArray(size_t size) {
      ar.resize(size);
      _used = size;
    }
    DynArray(T input[], size_t input_l): ar(input, input_l) {
      _used = input_l;
    }
    DynArray(DynArray<T>* input): ar(input->len()) {
      _used = 0;
      array_copy<DynArray<T>, DynArray<T>>(this, input);
    }
    void append(T data) {
      size_t length = ar.len();
      if (_used == length) {
        ar.resize(length == 0 ? 1 : length * 2);
      }
      ar[_used++] = data;
    }
    void push(T data) {
      append(data);
    }
    bool pop(T *val) {
      if (_used > 0) {
        _used--;
        *val = ar[_used];
        // Reclaim memory if the amount used gets small
        if (_used < ar.len()/2) {
          resize(_used);
        }
        return true;
      }
      return false;
    }
    void resize(size_t size) {
      if (size > ar.len() || size < ar.len()/2) {
        ar.resize(size);
      }
      _used = size;
    }
    void drop() {
      if (_used > 0) {
        _used--;
        // Reclaim memory if the amount used gets small
        if (_used < ar.len()/2) {
          resize(_used);
        }
      }
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
    size_t len() {
      return _used;
    }
    bool is_full() {
      return false;
    }
    size_t size() {
      return ar.len();
    }
    void swap(size_t i, size_t j) {
      ARRAY_CHECK(i);
      ARRAY_CHECK(j);
      T tmp = ar[i];
      ar[i] = ar[j];
      ar[j] = tmp;
    }
    void check(size_t index) {
      size_t length = ar.len();
      if (_used > length) {
        PANIC("Array, more elements used than exist\n");
      }
      if (index >= length) {
        PANIC("Array access out of bounds\n");
      }
      if (index >= _used) {
        PANIC("Array access of uninitialized data\n");
      }
    }
};

template<typename T, size_t Size>
class StaticArray {
  private:
    T ar[Size];
    size_t _used;
  public:
    StaticArray() {
      _used = 0;
    }
    StaticArray(T input[], size_t input_l) {
      if (input_l > Size) {
        PANIC("Static Array initializer is too long");
      }
      _used = input_l;
      // TODO: This should use memcpy
      for (size_t i=0; i<input_l; i++) {
        ar[i] = input[i];
      }
    }
    StaticArray(StaticArray<T, Size>* input) {
      array_copy<StaticArray<T, Size>, StaticArray<T, Size>>(this, input);
    }
    void append(T data) {
      ar[_used++] = data;
    }
    void push(T data) {
      append(data);
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
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(_used-index);
      return ar[_used-index];
    }
    size_t len() {
      return _used;
    }
    bool is_full() {
      return _used >= Size;
    }
    size_t size() {
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
    void check(size_t index) {
      if (_used > Size) {
        PANIC("Array, more elements used than exist\n");
      }
      if (index >= Size) {
        printf("Array access out of bounds index:%lu >= size:%lu\n", index, Size);
        PANIC("Array access out of bounds index");
      }
      if (index >= _used) {
        printf("Array access out of bounds index:%lu >= used:%lu\n", index, _used);
        PANIC("Array access of uninitialized data\n");
      }
    }
};

