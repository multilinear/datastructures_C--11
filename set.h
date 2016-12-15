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
    Set(Set<T> &s):tree() {
			*this = s;			
		}
    ~Set() {
			auto a = tree.begin();
			while (a != end() && remove(*a)) {
				a = begin();
			}
    }
    bool contains(T val) {
      return !!tree.get(val);
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
    bool operator[](T &key) {
      return tree.get(key) != nullptr;
    }
    typename BTree<T, T, SetComp, SET_ARITY>::Iterator begin() {
      return tree.begin();
    }
    typename BTree<T, T, SetComp, SET_ARITY>::Iterator end() { 
      return tree.end();
    }
		// Basic set operations
    Set<T>& operator-=(Set &s) {
      for (auto i = s.begin(); i != s.end(); ++i) {
        remove(*s);
      }
			return *this;
    }
    Set<T>& operator+=(Set &s) {
      for (auto i = s.begin(); i != s.end(); ++i) {
        insert(*s);
      }
			return *this;
    }
		Set<T> operator+(Set &s) {
			Set<T> n();
			n += *this;
			n += s;
			return s;
		}
		Set<T> operator-(Set &s) {
			Set<T> n();
			n += *this;
			n -= s;
			return s;
		}
    Set<T>& operator=(Set &s) {
			// dump it
			auto a = tree.begin();
			while (a != end() && remove(*a)) {
				a = begin();
			}
			// and copy the other one
      for (auto i = s.begin(); i != s.end(); ++i) {
        insert(*s);
      }
			return *this;
    }
};

#endif

