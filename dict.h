/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 
 * (this header added 2017-02-04)
 * 
 * This is a simple wrapper creating a "dict" type interface (key/value store).
 * It can be ported to use basically any of the various dictionary style
 * structures in this library, but has been implemented using the best for
 * for most use cases (the btree). 
 */ 

#include "btree.h"

#ifndef DICT_H
#define DICT_H

// See experiments/btree_arity_test for details on this number
#ifndef DICT_ARITY
#define DICT_ARITY 30 
#endif

// TODO: To use pairs like this dict should really support move semantics
// internally (as data moves about a btree quite often).
// performance could probably be increased significantly with this small change

template<typename KT, typename VT>
class Dict {
  private:
    class DictComp {
      public:
        static KT val(const std::pair<KT,VT> &el) {
          return el.first;
        }
        static int compare(KT v1, KT v2) {
          return v1-v2;
        }
    };
    BTree<std::pair<KT,VT>, KT, DictComp, DICT_ARITY> tree;
  public:
    Dict():tree() {}
    ~Dict() {
      auto a = tree.begin();
      VT val;
      while (a != end() && remove(a->first, &val)) {
        a = begin();
      }
    }
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
    bool isempty(void) const {
      return tree.isempty();
    }
    operator bool() const {
      return !tree.isempty();
    }
    VT& operator[](KT &key) {
      return *tree.get(key);
    }
    typename BTree<std::pair<KT,VT>, KT, DictComp, DICT_ARITY>::Iterator begin() {
      return tree.begin(); 
    }
    typename BTree<std::pair<KT,VT>, KT, DictComp, DICT_ARITY>::Iterator end() {
      return tree.end(); 
    }
};

#endif

