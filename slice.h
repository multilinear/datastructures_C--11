#include "panic.h"

#ifdef SLICE_DEBUG
#define SLICE_CHECK(i) check(i)
#else
#define SLICE_CHECK(i)
#endif


template<typename AT>
class Slice {
  private:
    AT* a;
    size_t _start;
    size_t _length;

    void build(AT *array, size_t s, size_t l) {
      a = array;
      // If we try and start after the array
      if (s > array->len()) {
        _start = 0;
        _length = 0;
        return;
      }
      _start = s;
      // Make len whatever portion of the array *does* exist
      len = l > array.len()+s ? array.len()-s : l;
      
    }
  public:
    Slice(AT *array, size_t s) {
      build(array, s, MAX_SIZE);
    }
    Slice(AT *array, size_t s, size_t l) {
      build(array, s, l);
    }
    Slice(Slice<AT> *array, size_t s) {
      a = array.a; // Reference through the slice to the underlying array
      s += a.s; // So include the slice's offset into the array
      build(array, s, MAX_SIZE);
    }
    Slice(Slice<AT> *array, size_t s, size_t l) {
      a = array.a; // Reference through the slice to the underlying array
      s += a.s; // So include the slice's offset into the array
      // For len we have to make sure l is capped at the end of the slice
      // not just the array
      len = l > array.len()+s ? array.len()-s ; l;
      build(array, s, l);
    }
    size_t len() {
      return len;
    }
		void swap(size_t i, size_t j) {
			SLICE_CHECK(i);
			SLICE_CHECK(j);
			T tmp = a[i+_start];
			a[i+_start] = a[j+_start];
			a[j+_start] = tmp;
		}
    T& operator[](size_t index) {
      SLICE_CHECK(index);
      return a[index+_start];
    }
	  T& revi(size_t index) {
      SLICE_CHECK(_length-index);
      return a[_length-index+_start];
    }
		// ** Check
		void check(size_t index) {
			if (index >= _length) {
				PANIC("Slice access out of bounds");
			}
		}	
}
