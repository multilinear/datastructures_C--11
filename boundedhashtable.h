/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * This is much like a standard open chaining hashtable implementation except
 * 1) It uses AVL trees instead of lists, this bounds worst case performance
 * for most operations
 * 2) Resizes rehash incrementally, thus bounding the worst-case performance
 * for resize as well. To make finding the next element constant time we 
 * additionally put elements in a list.
 * 3) "zeroing" of new tables is constant time instead of linear, see
 * zero_array.h for details
 * 
 * Assumption: malloc/free (not realloc or calloc) are constant time
 * Given this assumption:
 * All operations on BoundedHashTable have the same worst-case big O run-time
 * as an AVL tree, but with average case runtimes (hopefully) similar to that of
 * a hashtable
 *
 * This means worst case: O(log(n)) insert, remove, get
 * Average case: O(1) insert, remove, get
 *
 * resizes up when size is < x data it contains
 * resizes down when size is > 2x data it contains
 *
 * Algorithm:
 * This is basically a hashtable except for 3 tricks
 * #1: We use a
 */ 

#include <stdio.h>
#include <utility>
#include "panic.h"
#include "zero_array.h"
#include "avl.h"
#include "dlist.h"

#ifndef BOUNDED_HASHTABLE_H
#define BOUNDED_HASHTABLE_H

#define MINSIZE 4

template <typename Node_T, typename Val_T>
class FixedHashTable {
  private:
    ZeroArray<AVL<Node_T, Val_T>> table;
    DList<Node_T, Val_T> l;
    size_t next;
  public:
    typename DList<Node_T, Val_T>::Iterator begin(){
      return l.begin();
    }
    typename DList<Node_T, Val_T>::Iterator end(){
      return l.end();
    }
    FixedHashTable();
    ~FixedHashTable();
    void reset(size_t s);
    bool insert(Node_T *n);
    Node_T* get(Val_T key);
    Node_T* getOne(void);
    Node_T* remove(Node_T *n);
    void print(void);
    size_t len(void);
};

template <typename Node_T, typename Val_T>
FixedHashTable<Node_T,Val_T>::FixedHashTable() {
  // Warning: There's a trick happening here!! 
  // We create one empty AVL tree. That empty AVL tree
  // is *copied* in to a node whenever one is accessed
  // that hasn't been accessed before.
  // Then, we actually never call the destructor!
  // This is okay because an empty AVL tree has nothing to
  // free... but it's an API violation, so be careful 
  table.set_zero(AVL<Node_T, Val_T>());
}

template <typename Node_T, typename Val_T>
FixedHashTable<Node_T,Val_T>::~FixedHashTable() {};

template <typename Node_T, typename Val_T>
void FixedHashTable<Node_T,Val_T>::reset(size_t s) {
  table.reset(s); // this is constant time
}
 

template <typename Node_T, typename Val_T>
bool FixedHashTable<Node_T, Val_T>::insert(Node_T *new_node) {
  // Hash it
  size_t i = Node_T::hash(new_node->val()) % table.len();
  bool b = table[i].insert(new_node); 
  if (b) {
    l.insert(new_node);
  }
  return b;
}

template <typename Node_T, typename Val_T>
Node_T* FixedHashTable<Node_T,Val_T>::get(Val_T key) {
  size_t i = Node_T::hash(key) % table.len();
  return table[i].get(key);
}

template <typename Node_T, typename Val_T>
Node_T* FixedHashTable<Node_T,Val_T>::getOne(void) {
  return l.peak();
}

template <typename Node_T, typename Val_T>
Node_T* FixedHashTable<Node_T,Val_T>::remove(Node_T *n) {
  Val_T v = n->val();
  size_t i = Node_T::hash(v) % table.len();
  table[i].remove(n);
  l.remove(n);
  return n;
}

template <typename Node_T, typename Val_T>
void FixedHashTable<Node_T,Val_T>::print(void) {
  printf("[\n");
  for (size_t i=0; i<table.len(); ++i) {
    printf("  ");
    table[i].print();
  }
  printf("]\n");
}
 
template <typename Node_T, typename Val_T>
size_t FixedHashTable<Node_T,Val_T>::len(void) {
  return table.len();
}

template <typename Node_T, typename Val_T>
class BoundedHashTable {
  private:
    FixedHashTable<Node_T, Val_T> at1;
    FixedHashTable<Node_T, Val_T> at2;
    FixedHashTable<Node_T, Val_T> *t1;
    FixedHashTable<Node_T, Val_T> *t2;

    size_t count;
    void resize(size_t s) {
      if (t2->getOne()) {
        PANIC("t2 not empty and we're trying to reset!\n");
      }
      t2->reset(s);
      // Swap the tables
      auto tmp = t2;
      t2 = t1;
      t1 = tmp;
    }
    void inc() {
      // Frumious downcast... This lets us get around
      // issues with multiple inheritance
      Node_T *n = t2->getOne();
      if (n) {
        t2->remove(n);
        n->ht = t1;
        t1->insert(n);
      }
    }
  public:
    class Iterator {
      private:
        typename DList<Node_T, Val_T>::Iterator it;
        typename DList<Node_T, Val_T>::Iterator jump;
        typename DList<Node_T, Val_T>::Iterator jumpto;
        bool jumped;
      public:
        Iterator(typename DList<Node_T, Val_T>::Iterator _it,
            typename DList<Node_T, Val_T>::Iterator _jump,
            typename DList<Node_T, Val_T>::Iterator _jumpto, 
            bool _jumped) {
          it = _it;
          jump = _jump;
          jumpto = _jumpto;
          jumped = _jumped;
        }
        Iterator(const Iterator& other) {
          it = other.it;
          jump = other.jump;
          jumpto = other.jumpto;
          jumped = other.jumped;
        }
        Iterator& operator=(const Iterator& other) {
          it = other.it;
          jump = other.jump;
          jumpto = other.jumpto;
          jumped = other.jumped;
          return *this;
        }
        bool operator==(const Iterator& other) {
          return it == other.it;
        }
        bool operator!=(const Iterator& other) {
          return !((*this) == other);
        }
        Iterator operator++() {
          it++;
          if (!jumped && it == jump) {
            jumped = true;
            it = jumpto;
          }
          return *this;
        }
        Iterator operator++(int) {
          Iterator tmp(*this);
          ++(*this);
          return tmp;
        }
        Node_T& operator*() {
					// Get what's inside the iterator
          return *it;
        }
        Node_T* operator->() {
					// Get a reference to what's inside the iterator (lol)
          return &(*it);
        }
    };
    Iterator begin() {
      return Iterator(t1->begin(), t1->end(), t2->begin(), false);
    }
    Iterator end() {
      return Iterator(t2->end(), t1->end(), t2->begin(), true);
    }

    BoundedHashTable() {
      count = 0;
      t1 = &at1;
      t2 = &at2;
      t1->reset(MINSIZE);
      t2->reset(MINSIZE);
    };
    ~BoundedHashTable() {}
    bool insert(Node_T *n) {
      inc();
      // if it's over full resize up
      if (t1->len() < count+1) {
        resize(t1->len()*2);
      }
      // Make sure it's not in t2 already
      if (t2->get(n->val())) {
        return false;
      }
      n->ht = t1;
      if(t1->insert(n)) {
        count++;
        return true;
      }
      return false;
    }
    Node_T* get(Val_T key) {
      Node_T *n = t1->get(key);
      if (!n) {
        n = t2->get(key);
      }
      return n;
    }
    Node_T* remove(Node_T *n) {
      // Only half the elements have to be removed
      // before we resize, but all have to be
      // out of t2, so we call inc() twice
      inc();
      inc();
      n->ht->remove(n);
      count--;
      // if it's under half full resize down
      if (t1->len() > 2*count && !t2->getOne()) {
        resize(t1->len()/2);
      }
      return n;
    }
    bool isempty(void) const {
      return count == 0;
    }
    void print() {
      printf("[");
      printf("t1=");
      t1->print();
      printf("t2=");
      t2->print();
      printf("]\n");
    }
};

// We pull a bit of a frumious hack here
// Note the multiple inheritance, to give DList access to the DListNode data,
// we actually upcast to that type before putting it in the list, then downcast
// again when we take it out...
// This emulates the behavior of a classic C-style externally allocated structure
// with less macro magic, at the cost of a little loss in type-checking.
template <typename Node_T, typename Val_T>
class BoundedHashTableNode_base: public AVLNode_base<Node_T, Val_T>, public DListNode_base<Node_T> {
  public:
    FixedHashTable<Node_T,Val_T> *ht;
    // subclass must implement:
    // Val_T val(void);
    // void print(void);
    // static size_t hash(Val_T v);
};

#endif
