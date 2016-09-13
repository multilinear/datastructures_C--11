#include "panic.h"

#ifdef ARRAY_DEBUG
#define ARRAY_CHECK(i) check(i)
#else
#define ARRAY_CHECK(i)
#endif

template<typename T>
class Array {
  private:
    T *ar;
    size_t _length;
  public:
    Array() {
      ar = nullptr;
      resize(10);
    }
    ~Array() {
      free(ar);
    }
    void resize(size_t new_size) {
      _length = new_size;
      ar = (T*) realloc(ar, _length * sizeof(T));
    }
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(_length-index);
      return ar[_length-index];
    }
    size_t len() {
      return _length;
    }
    void check(size_t index) {
      if (index >= _length) {
        PANIC("Array access out of bounds\n");
      }
    }
};

template<typename T>
class UsedArray {
  private:
    Array<T> ar;
    size_t _used;
  public:
    UsedArray() {
      _used = 0;
    }
    void append(T& data) {
      size_t length = ar.len();
      if (_used == length) {
        ar.resize(length * 2);
      }
      ar[_used++] = data;
    }
    void push (T& data) {
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
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(_used-index);
      return ar[_used-index];
    }
    size_t used () {
      return _used;
    }
    size_t is_empty() {
      return _used == 0;
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
    void append(T& data) {
      ar[_used++] = data;
    }
    void push(T& data) {
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
      ARRAY_CHECK(_used-1);
      _used--;
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
    size_t used() {
      return _used;
    }
    bool is_full() {
      return _used >= Size;
    }
    bool is_empty() {
      return _used == 0;
    }
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

