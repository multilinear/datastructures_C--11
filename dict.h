#include "btree.h"

#ifndef DICT_H
#define DICT_H

// See experiments/btree_arity_test for details on this number
#ifndef DICT_ARITY
#define DICT_ARITY 30 
#endif

// TODO(mbrewer): should this be a storing pointers, not pairs?
template<typename KT, typename VT>
class Dict {
  private:
    class DictComp {
      public:
        static KT val(std::pair<KT,VT> el) {
          return el.first;
        }
        static int compare(KT v1, KT v2) {
          return v1-v2;
        }
    };
    BTree<std::pair<KT,VT>, KT, DictComp, DICT_ARITY> tree;
  public:
    Dict():tree() {}
    VT* get(KT val) {
      auto ptr = tree.get(val);
      if (ptr) {
        return &(ptr->second);
      }
      return nullptr;
    }
    void set(KT key, VT value) {
      auto *el = tree.get(key);
      if (el) {
        el->second = value;
      } else {
        insert(key, value);
      }
    }
    bool insert(KT key, VT value) {
      return tree.insert(std::pair<KT,VT>(key,value));
    }
    bool remove(KT key, VT *result) {
      std::pair<KT,VT> pair;
      bool found = tree.remove(key, &pair);
      if (found) {
        *result = pair.second;
      }
      return found;
    }
    bool isempty(void) {
      return tree.isempty();
    }
};

#endif

