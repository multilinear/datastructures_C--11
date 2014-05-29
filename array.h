#include "panic.h"

template<typename T>
class Array {
  private:
    T *ar;
    size_t length;
  public:
    Array() {
      ar = nullptr;
      resize(10);
    }
    ~Array() {
      free(ar);
    }
    void resize(size_t new_size) {
      length = new_size;
      ar = (T*) realloc(ar, length * sizeof(T));
    }
    T& operator[](size_t index) {
      // Automatically resize for the last element
      if (index == length) {
        // take length and 
        resize((int) (length*2));
      } else if (index > length) {
        PANIC("Array access out of bounds\n");
      }
      return ar[index];
    }
    size_t len() {
      return length;
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
      ar[_used++] = data;
    }
    void push (T& data) {
      append(data);
    }
    T pop() {
      return ar[--_used];
    }
    T& operator[](size_t index) {
      if (index < _used) {
        return ar[index];
      }
      PANIC("ArrayList access out of bounds\n"); 
    }
    size_t used () {
      return _used;
    }
};

