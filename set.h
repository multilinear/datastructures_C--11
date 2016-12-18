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
    Set(const Set<T> &s):tree() {
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

    Set& operator=(const Set &s) {
			// dump it
			auto a = tree.begin();
			while (a != end() && remove(*a)) {
				a = begin();
			}
      // and add in the new data
			return (*this += s);
    }

    // For use like an array
    bool operator[](T &key) const {
      return tree.get(key) != nullptr;
    }
    typename BTree<T, T, SetComp, SET_ARITY>::Iterator begin() const {
      return tree.begin();
    }
    typename BTree<T, T, SetComp, SET_ARITY>::Iterator end() const { 
      return tree.end();
    }
    operator bool() {
      return !tree.isempty();
    }

    // ** comparisons
    bool operator==(const Set &s) const {
      auto i = s.begin(); 
      auto j = begin(); 
      while(i != s.end() && j != end()) {
        if (*i != *j) {
          return false;
        }
        ++i; 
        ++j;
      }
      return i == s.end() && j == end(); 
    }
    bool operator!=(const Set &s) const {
      return !(*this==s);
    }

		// ** set operations
    // sutraction
    Set& operator-=(const Set &s) {
      for (auto i = s.begin(); i != s.end(); ++i) {
        remove(*i);
      }
			return *this;
    }
    // union
    Set& operator+=(const Set &s) {
      for (auto i = s.begin(); i != s.end(); ++i) {
        insert(*i);
      }
			return *this;
    }
    // intersection
    Set& operator&=(const Set &s) {
      // TODO(mbrewer): This uses more memory and copies than it should
      // we can't modify "this" while iterating over it, so the obvious
      // removal based algorithm doesn't work
      Set<T> n((*this) & s);
      *this = n;
      return *this;
    }
    // also union
    Set& operator|=(const Set &s) {
      return (*this)+=s;
    }
    // co-intersection
    Set& operator^=(const Set &s) {
      for (auto i = begin(); i != end(); ++i) {
        if (s[*i]) {
          remove(*i);
        } else {
          insert(*i);
        }
      }     
      return *this;
    }
		Set operator+(const Set &s) const {
			Set<T> n(*this);
			n += s;
			return n;
		}
		Set operator-(const Set &s) const {
			Set<T> n(*this);
			n -= s;
			return n;
		}
		Set operator&(const Set &s) const {
			Set<T> n;
      for (auto i = begin(); i != end(); ++i) {
        if (s[*i]) {
          n.insert(*i);
        }
      }
      return n;
		}
		Set operator|(const Set &s) const {
			Set<T> n(*this);
			n |= s;
			return n;
		}
		Set operator^(const Set &s) const {
			Set<T> n(*this);
			n ^= s;
			return n;
		}
};

#endif

