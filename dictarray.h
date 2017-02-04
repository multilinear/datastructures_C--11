/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 2014-12-21
 *
 * When to use this:
 * For normal operations this is ludicrously slow, don't use it.
 * I've left it here for 2 reasons:
 * 1) It lazilly allocates cells, so might be useful for extremely
 * sparse data-sets while still using normal array-based algorithms
 * 2) For demonstration and testing purposes
 *
 * Design Decisions:
 * Resizing an array sometimes requires copying the memory
 * this can be a problem if you care about the wort-case
 * runtime of an algorithm.
 * It does work that extent. Strictly speaking it makes every
 * operation logarithmic in return for making the worst case
 * logarithmic instead of linear. 
 * That said, the constant factors are likely too poor for
 * this to be generally all that useful, outside of say
 * realtime applications. I'm still persuing better options.
 *
 * The one possible use-case:
 * This could be used as what is known as a "sparse array" If you want
 * to do array style computation over a very large space, where there
 * won't be much actual data in it, this could work very well. This
 * implementation actually lazilly allocates cells when they are accessed
 *
 * Threadsafety:
 *   thread compatible
 */



#include "btree.h"
      
#ifndef DICTARRAY_H
#define DICTARRAY_H
      
// See experiments/btree_arity_test for details on this number
#ifndef DICTARRAY_ARITY
#define DICTARRAY_ARITY 30 
#endif

template<typename T>
class DictUArray {
  private:
    // private class
    class DictUArrayComp {
      public:
        static size_t val(const std::pair<size_t,T> &el) {
          return el.first;
        }
        static int compare(size_t v1, size_t v2) {
          return v1-v2;
        }
    };

    // Private Variables
    size_t used;
    BTree<std::pair<size_t,T>, size_t, DictUArrayComp, DICTARRAY_ARITY> tree;

    // private helper functions
    T& get(size_t i) {
      auto ptr = tree.get(i);
      if (ptr) {
        return (ptr->second);
      }
      tree.insert(std::pair<size_t,T>(i,0));
      return tree.get(i)->second;
    }
  public:
    DictUArray():tree() {
      used = 0; 
    }
    DictUArray(T input[], size_t input_l):tree() {
      used = 0; 
      for (size_t i=0; i<input_l; i++) {
        push(input[i]);
      }
    }
    ~DictUArray() {
			std::pair<size_t,T> val;
			auto a = tree.begin();
      while (a != tree.end() && tree.remove(a->first, &val)) {
				// iterator is invalidated due to remove, so start over
    		a = tree.begin();
			}
    }
		T& operator[](size_t i) {
      return get(i);
    }
    T& revi(size_t i) {
      return get(used-i);
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
    bool push(T data){    
      get(used) = data;
      used++;
      return true;
    }
    bool pop(T *data) {
      if (used == 0) {
        return false;
      }
      *data = get(used-1);
      used--;
      return true;
    }
    void drop() {
      if (used > 0) {
        used--;
      }
    }
    void resize(size_t size) { }
    void swap(size_t i, size_t j) {
      T tmp = get(i);
      get(i) = get(j);
      get(j) = tmp;
    }
};

#endif
