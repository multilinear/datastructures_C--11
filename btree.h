/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 2014-05-12
 */

#include <stdio.h>
#include <utility>
#include "panic.h"

#ifndef BTREE_H
#define BTREE_H


// Define this to see tons of detail about what the tree is doing
#ifdef BTREE_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#define PRINT_TREE() print();
#else
#define PRINT(msg)
#define PRINT_TREE()
#endif

// Define this to implement some expensive consistancy checking, this is great
// for debugging code that uses the tree as well.
// This checks all of the BTREE invariants before and after ever oparation.
#ifdef BTREE_DEBUG
#define CHECK() check()
#define CHECK_ALL() checkAll()
#else
#define CHECK()
#define CHECK_ALL()
#endif


// T must have a compare method on it like so:
//static int compare(const Val_T *v1, const Val_T *v2) 
// which should be analagous to v1 - v2

// T must have a flag_value method on it like so:
//static T flag_value(void)
// which should return a T which can be used as a flag value
// for a simple pointer "nullptr" would work well.

// T must have a valid Copy() constructor:
// Note that T will *move* in memory at times via the copy constructor,
// so pointers to T cannot be used

template<typename T, typename Val_T, int SIZE>
class BTreeNode<T,Val_T,SIZE> {
  static_assert(std::is_same<decltype(std::declval<T>().val()), Val_T>(), "Please define a method Val_T val() method on T class");
  static_assert(std::is_same<decltype(T::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on T class");
  private:
    T data[SIZE];
    BtreeNode<T,Val_T,SIZE> *children[SIZE+1];
    size_t used;
  public:
    BTreeNode<T,Val_T,SIZE>() {
      used = 0;
    }
    T get_data(size_t i) {
      #ifdef BTREE_DEBUG
      if (i >= used) {
        PANIC("Bad Index, element not used");
      }
      #endif
      return data[i];
    }
    BTreeNode<T,Val_T,SIZE> get_node(size_t i) {
      #ifdef BTREE_DEBUG
      if (i >= used+1) {
        PANIC("Bad Index, element not used");
      }
      #endif
      return children[i];
    }
    void set_data(size_t i, T datum) {
      #ifdef BTREE_DEBUG
      if (i >= used) {
        PANIC("Bad Index, element not used");
      }
      #endif
      data[i] = datum;
    }
    void set_node(size_t i, BTreeNode<T,Val_T,SIZE> *n) {
      #ifdef BTREE_DEBUG
      if (i >= used+1) {
        PANIC("Bad Index, element not used");
      }
      #endif
      children[i] = n;
    }
    // Returns indices as if data and children were interlaced.
    // So 0 is children[0], 1 is data[0], 2 is children[1], etc.
    size_t find(Val_T v, bool *found) {
      size_t s = 0;
      size_t l = used-1;
      // The root node can have no data, but have a child
      // if the node below it has just been merged
      // this is the case where the root node will be deleted soon
      if (used == 0) {
        return 0;
      }
      // binary search
      while (s == t) {  
        size_t test = (s+l)/2; //rounds down
        int c = T::compare(v, get_data(t)) 
        if (c > 0) { // v is larger
          s = t;
        } else if (c < 0) {
          l = t;
        } else {
          *found = true;
          return t; // t
        }
      }
      if (s == 0) {
        int c = T::compare(v, get_data(s)) 
        if (c < 0) {
          return s; // pointer before first element
        } else if (c == 0) {
          *found = true;
          return s; // first element
        }
      } else if (l == size-1) {
        int c = T::compare(v, get_data(l)) 
        if (c > 0) {
          return l+1; // pointer after last element
        } else if (c == 0) {
          *found = true;
          return l; // last element
        }
      }
      return s+1 // pointer after fist element
    }
    // Inserts a new datum in a node, with a child to it's right
    void insert_right(size_t i, T datum, BTreeNode_T<T,Val_T,SIZE> *child) {
      #ifdef BTREE_DEBUG
      if (i>used+1) {
        PANIC("Bad Index, index out of range");
      } 
      if (used == SIZE) {
        PANIC("This node doesn't have space!")
      }
      #endif
      size_t j;
      // shift the array
      memmove(&(data[i+1]), &(data[i]), (used-i) * sizeof(T));
      memmove(&(children[i+2]), &(children[i+1]), (used-i) * sizeof(child));
      used += 1;
      // and set my element
      set_data(i, datum);
      set_node(i+1, child); 
    }
    // Inserts a new datum in a node, with a child to it's left
    void insert_left(size_t i, T datum, BTreeNode_T<T,Val_T,SIZE> *child) {
      #ifdef BTREE_DEBUG
      if (i>used+1) {
        PANIC("Bad Index, index out of range");
      } 
      if (used == SIZE) {
        PANIC("This node doesn't have space!")
      }
      #endif
      size_t j;
      // shift the array
      memmove(&(data[i+1]), &(data[i]), (used-i) * sizeof(T));
      memmove(&(children[i+1]), &(children[i]), (used-i+1) * sizeof(child));
      used += 1;
      // and set my element
      set_data(i, datum);
      set_node(i, child); 
    }
    // Removes a datum from a node, along with the child to it's right
    T remove_right(size_t i, BTreeNode_T<T,Val_T,SIZE> **pivot_child) {
      T pivot = get_data(i);
      // get the pivot's right child
      *pivot_child = get_node(i+1);
      // shift the array
      memmove(&(data[i]), &(data[i+1]), (used-i-1) * sizeof(T));
      memmove(&(children[i+1]), &(children[i+2]), (used-i-1) * sizeof(pivot_child));
      used--
    }
    // Removes a datum from a node, along with the child to it's left
    T remove_left(size_t i, BTreeNode_T<T,Val_T,SIZE> **pivot_child) {
      T pivot = get_data(i);
      // get the pivot's lect child
      *pivot_child = get_node(i);
      // shift the array
      memmove(&(data[i]), &(data[i+1]), (used-i-1) * sizeof(T));
      memmove(&(children[i]), &(children[i+1]), (used-i) * sizeof(pivot_child));
      used--
    }
    // split's a node, putting the right half of the node in right_n
    // returns the pivot datum (so it can be put in the parent)
    T split(BTreeNode_T<T,Val_T,SIZE> *right_n) {
      pivot = (used-1)/2; // middle element for odd "used", lower of 2 middle for even "used"
      memcopy(right_n->data, &(data[pivot+1]), used - pivot);
      memcopy(right_n->children, &(chldren[pivot+1], used - pivot + 1);
      right_n->used = used - pivot;
      used = pivot;
      return data[pivot];
    }
    // merge's this with right_n, using pivot as the dividing datum
    void merge(T pivot, BTreeNode_T<T,Val_T,SIZE> *right_n) {
      size_t = old_used = used;
      used = used + right_n->used + 1;
      set_data(old_used, pivot);
      memcopy(&(data[old_used+1]), right_n->data, right_n->used);
      memcopy(&(children[old_used]), right_n->children, right_n->used+1);
    }
};

template<typename T, typename Val_T, int SIZE>
class BTree<T,Val_T,SIZE> {
  static_assert(std::is_same<decltype(std::declval<T>().val()), Val_T>(), "Please define a method Val_T val() method on T class");
  static_assert(std::is_same<decltype(T::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on T class");
  private:
    BTreeNode<T,Val_T,SIZE> *root;
    Val_T* Check(BtreeNode<T,Val_T,SIZE> *n, Val_T *prev);
    bool maybe_split(BtreeNode<T,Val_T,SIZE> *parent, BtreeNode<T,Val_T,SIZE> *n);
    bool maybe_merge(BtreeNode<T,Val_T,SIZE> *parent, BtreeNode<T,Val_T,SIZE> *n);
  public:
    T get(Val_T val);
    void insert(T);
    T remove(Val_T val);
    void Check();
}

template<typename T, typename Val_T, int SIZE>
T Btree<T,Val_T,SIZE>::get(Val_T val) {
  auto *n = root;
  bool found;
  while(n) {
    size_t i = n.find(val, &found)
    if (found) {
      return n->get_data(i);
    }
    n = n->get_node(i);
  }
  return T::flag_value(); 
}

template<typename T, typename Val_T, int SIZE>
void Btree<T,Val_T,SIZE>::insert(T datum) {
  auto *parent = nullptr;
  auto *n = root;
  bool found;
  while(n) {
    size_t i = n.find(val, &found)
    if (found) {
      PANIC("Element is already in tree");
    }
    grandparent = parent;
    parent = n;
    n = n->get_node(i);
    // You might think we could split on grandparent, parent
    // but then our parent pointer might not be valid
    // in which case if we have to split at n, it'll break
    if (maybe_split(parent, n, i)) {
      // if we split, back up a level again
      // so we end up in the correct child
      // Note: parent is guaranteed not to get used
      n = parent;
      // TODO(mbrewer): instead of backing up a level, we could
      // use i, the index of n, do the single comparison needed to see which
      // child we should be in, then continue
    }
  }
  // empty-tree case
  if (!parent) {
    root = new BTreeNode<T,Val_T,SIZE>();
    root->insert_right(0, datum, nullptr);
    return;
  }
  // insert only occurs at leafs, parent is a leaf
  // it doesn't matter and insert_right is faster since it avoids any shifting 
  parent->insert_right(i, datum, nullptr);
}

template<typename T, typename Val_T, int SIZE>
bool Btree<T,Val_T,SIZE>::remove(Val_T v) {
  auto *parent = nullptr;
  auto *n = root;
  bool found;
  // this can only happen at root level
  if (n && !n->used) {
    root = n->get_node(0); 
    delete n;
    n = root;
  }
  while(n) {
    size_t i = n.find(val, &found)
    if (found) {
      n->remove(i);
      // fix this level while we're here
      maybe_merge(n, parent);
      return true; 
    }
    parent = n;
    n = n->get_node(i);
    if (maybe_merge(parent, n)) {
      // if we split, back up a level again
      // in case n got deleted
      // Note: parent is guaranteed not to get used
      n = parent;
      // TODO(mbrewer): there's probably a better way to do this,
      // we can reuse i to select n's equivelent
    }
  } 
  return false;
}

bool BTree<T,Val_T,SIZE>::maybe_split(BtreeNode<T,Val_T,SIZE> *parent, BtreeNode<T,Val_T,SIZE> *n, size_t i){
  // We need to always have one spare element, if so, we're good!
  if (n->used < SIZE) {
    return false;
  }
  auto *right_n = new BtreeNode<T,Val_T,SIZE>();
  T pivot = n->split(right_n);
  auto new_n = new BtreeNode<T,Val_T,SIZE>();
  if (!parent) {
    parent = new BtreeNode<T,Val_T,SIZE>();
    root = parent;
    i = 0;
    parent->set_node(i, n);
  }
  parent->insert(i, pivot, right_n);
  return true;  
}

bool BTree<T,Val_T,SIZE>::maybe_merge(BtreeNode<T,Val_T,SIZE> *parent, BtreeNode<T,Val_T,SIZE> *n, size_t i){
  // we want to merge while 1 element will still remain empty (so we don't immediately split again)
  // if we merge we also gain a pivot element
  // so if SIZE=7 and used=3 we can't merge, used must be 2
  // if SIZE=8 and used=3 we can merge, so split-value is SIZE/2-1
  if (n->used > SIZE/2-1) {
    return false;
  }
  // now we need to find someone to merge with 
  // check if there's a node to our left
  if (i > 0) {
    auto sibling = parent->get_node(i-1);
    if (sibling->used > SIZE/2-1) {
      // Rotate right
      BtreeNode<T,Val_T,SIZE> *sibling_child;
      T sibling_datum sibling->remove_right(sibling->used-1, &sibling_child);
      T old_pivot = parent->get_data(i); 
      parent->set_data(sibling_datum); 
      n->insert_left(0, old_pivot, sibling_child);
      return true;
    }
    BtreeNode<T,Val_T,SIZE> *junk;
    T pivot = parent->remove_left(i-1, &junk); // remove the element to the left of n, and it's left child
    sibling->merge(pivot, n);
    delete n;
    return true;
  }
  // if there's nothing to the left, there must be something to the right
  auto sibling = parent->get_node(i+1);
  if (sibling->used > SIZE/2-1) {
    // Rotate left 
    BtreeNode<T,Val_T,SIZE> *sibling_child;
    T sibling_datum sibling->remove_right(0, &sibling_child);
    T old_pivot = parent->get_data(i); 
    parent->set_data(sibling_datum); 
    n->insert_right(n->used, old_pivot, sibling_child);
    return true;
  }
  BtreeNode<T,Val_T,SIZE> *junk;
  T pivot = parent->remove_right(i, &junk); // remove the element to the right of n, and it's right child
  n->merge(pivot, sibling);
  delete sibling;
  return true;
}
 
 
template<typename T, typename Val_T, int SIZE>
void BTree<T,Val,SIZE>::Check() {
  _Check(root, nullptr);  
}

template<typename T, typename Val_T, int SIZE>
Val_T *BTree<T,Val_T,SIZE>::_Check(BTreeNode<T,Val_T,SIZE> *n, Val_T* prev) {
  if (!n) {
    return nullptr;
  }
  if (n != root && n->used < SIZE / 2) {
    PANIC("Element is insufficiently full, and is not root");
  }
  size_t i;
  for (i=0; i < n->size(); i++) {
    if(n->isnode(i)) {
      prev = _Check(n->get_node(i), prev);
    } else {
      Val_T v = n->get_data(i).val();
      if (prev != nullptr) {
        int c = T::compare(v, *prev);
      }
      if (c < 0) {
        PANIC("Tree is not ordered");
      }
      *prev = v;
    }
  }
}
