#include "btree.h"

#ifndef SET_H
#define SET_H

// See experiments/btree_arity_test for details on this number
#define SET_ARITY 55

template<typename T>
class Set {
  private:
    class SetComp {
      public:
        static T val(T el) {
          return el;
        }
        static int compare(T v1, T v2) {
          return v1-v2;
        }
    };
    BTree<T, T, SetComp, SET_ARITY> tree;
  public:
    Set():tree() {}
    T* get(T val) {
      return tree.get(val);
    }
    bool insert(T val) {
      return tree.insert(val);
    }
    bool remove(T val) {
      T ret;
      // TODO(mbrewer): consider a version of remove in btree that doesn't copy
      // the value
      return tree.remove(val, &ret);
    }
    bool isempty(void) {
      return tree.isempty();
    }
};

#endif

