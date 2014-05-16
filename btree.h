/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 2014-05-12
 *
 * This is an implementation of a binary B-tree, with lazy split/merge.
 * If SIZE is set to 5, this implements a 2-3-4 tree with lazy split/merge.
 *
 * In this configuration, the tree is quite performant. See PERFORMANCE
 *
 * Design Decisions:
 *   Lazy split/merge: if SIZE=5, then 1 and 5 are legal sizes temporarilly.
 * When we walk down the tree for a remove(), we check for merges as we go
 * This guarantees that the node we operate on, and all those above it, have
 * at least 2 elements in them, as they should, thus we don't suddenly end up
 * going farther out of bounds.
 * On Insert we do the same thing with splits/rotations. Ensuring our invariants
 * hold for all nodes on the path to the node we insert into.
 * The advantage of this approach, is there's no need to recurse back up the
 * tree. All operations can simply walk down the tree, and terminate.
 *
 *   Why the horrific template?
 * I was trying to get speeds up. With this implementation if "T", the data
 * stored in the tree, is a simple "int" we incur no extra costs, for
 * construction and the like, yet compare() and val() get included statically
 * and thus can be inlined out.
 * We use "val()" so that if T is a pointer to some structure it's still efficient
 * You can pass key's in (of type Val_T), for searches, e.g. a tid for a tree of
 * thread structures. Again val() and compare() can be inlined.
 * If for some reason you *want* to store entire classes in the B-tree it will
 * work. If you do this you may want to go through making sure all the T's are T&
 * so you don't call the copy constructor over and over and over again.
 * Also, if T is complex read the requirements below very carefully.
 *
 */

#include <cstring>
#include <stdio.h>
#include <utility>
#include "panic.h"

#ifndef BTREE_H
#define BTREE_H

//#define BTREE_DEBUG_VERBOSE

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
#else
#define CHECK()
#endif

// T is intended to be something *very* simple. It must be a class but it's
// expected to contain a pointer, an integer, or maybe 2 or 3 items at most
// If you need to store something more complex, make T a class that contains
// just a pointer to your more complex object

// Below are the requirements for T, Val_T, C, and SIZE

// T must have a valid, trivial, Copy() constructor:
// Note that T will *move* in memory at times via the copy constructor,
// so pointers to T cannot be used
// also we will move it in memory *without* the copy constructor using memmove/memcpy
//
// T must have a print method on it like so:
// void print(void)
// which prints something for the element ("?" is find, but will make debugging harder)
// I also suggest it be small and have no newlines

// Val_T is simply whatever is returned by "Val_T C::val(T)", the point is for it to be easilly
// comparable, and small/cheap to toss around. An integral type of appropriate size for
// your problem is probably what you want, though it can be anything.

// C must have a compare method on it like so:
// static int compare(const Val_T *v1, const Val_T *v2) 
// which should be analagous to v1 - v2
//
// C must have a val method on it like so:
// static Val_T val(T &v1) 
// whith returns Val_T, that is whatever component of T is to be used for compare()

// SIZE must be at least 5
// if SIZE=4 then splitting the node creates an element of size 1. This gets
// awkward because lazy deletion would then require an element of size 0 to be
// legal temporarilly. We want nodes to have at least 1 element in them (excepting root
// which can transitionally be empty).

template<typename T, typename Val_T, typename C, int SIZE>
class BTreeNode {
  private:
    T data[SIZE];
    BTreeNode<T,Val_T,C,SIZE> *children[SIZE+1];
    size_t used;
  public:
    void print(void) {
      printf("[");
      size_t i;
      for (i=0; i<used; i++) {
        if(children[i]) {
          printf("c");
        } else {
          printf("n");
        }
        printf(",");
        C::printT(data[i]);
        printf(",");
      }
      if(children[used]) {
        printf("c");
      } else {
        printf("n");
      }
      printf("]");
    }
    BTreeNode<T,Val_T,C,SIZE>() {
      used = 0;
    }
    T get_data(size_t i) {
      #ifdef BTREE_DEBUG
      if (i >= used) {
        printf("i = %ld, used = %ld\n", i, used);
        PANIC("Bad Index, datum is empty");
      }
      #endif
      return data[i];
    }
    BTreeNode<T,Val_T,C,SIZE> *get_node(size_t i) {
      #ifdef BTREE_DEBUG
      if (i >= used+1) {
        printf("i = %ld, used = %ld\n", i, used);
        PANIC("Bad Index, child is empty");
      }
      #endif
      return children[i];
    }
    void set_data(size_t i, T datum) {
      #ifdef BTREE_DEBUG
      if (i >= used) {
        printf("i = %ld, used = %ld\n", i, used);
        PANIC("Bad Index, datum is empty");
      }
      #endif
      data[i] = datum;
    }
    void set_node(size_t i, BTreeNode<T,Val_T,C,SIZE> *n) {
      #ifdef BTREE_DEBUG
      if (i >= used+1) {
        printf("i = %ld, used = %ld\n", i, used);
        PANIC("Bad Index, child is empty");
      }
      #endif
      children[i] = n;
    }
    size_t get_used() {
      return used;
    }
    // Returns indices as if data and children were interlaced.
    // So 0 is children[0], 1 is data[0], 2 is children[1], etc.
    size_t find(Val_T v, bool *found) {
      *found = false;
      size_t s = 0;
      size_t l = used-1;
      // The root node can have no data, but have a child
      // if the node below it has just been merged.
      // This is the case where the root node will be deleted soon
      if (used == 0) {
        return 0;
      }
      // binary search
      size_t test = (s+l)/2; // rounds down
      while (s != test) {  
        int c = C::compare(v, C::val(get_data(test)));
        if (c > 0) { // v is larger
          s = test;
        } else if (c < 0) {
          l = test;
        } else {
          *found = true;
          return test;
        }
        test = (s+l)/2; //rounds down
      }
      if (s == 0) {
        // Outlier case, it might be *less* than s
        int c = C::compare(v, C::val(get_data(s)));
        if (c < 0) {
          return s; // pointer before first element
        } else if (c == 0) {
          *found = true;
          return s; // first element
        }
      }
      if (l == used-1) {
        // Outlier case, it might be *greater* than l
        int c = C::compare(v, C::val(get_data(l)));
        if (c > 0) {
          return l+1; // pointer after last element
        } else if (c == 0) {
          *found = true;
          return l; // last element
        }
      }
      return s+1; // pointer after fist element
    }
    // Inserts a new datum in a node, with a child to it's right
    void insert_right(size_t i, T datum, BTreeNode<T,Val_T,C,SIZE> *child) {
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
    void insert_left(size_t i, T datum, BTreeNode<T,Val_T,C,SIZE> *child) {
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
    T remove_right(size_t i, BTreeNode<T,Val_T,C,SIZE> **pivot_child) {
      T pivot = get_data(i);
      // get the pivot's right child
      *pivot_child = get_node(i+1);
      // shift the array
      memmove(&(data[i]), &(data[i+1]), (used-i-1) * sizeof(T));
      memmove(&(children[i+1]), &(children[i+2]), (used-i-1) * sizeof(pivot_child));
      used--;
      return pivot;
    }
    // Removes a datum from a node, along with the child to it's left
    T remove_left(size_t i, BTreeNode<T,Val_T,C,SIZE> **pivot_child) {
      T pivot = get_data(i);
      // get the pivot's lect child
      *pivot_child = get_node(i);
      // shift the array
      memmove(&(data[i]), &(data[i+1]), (used-i-1) * sizeof(T));
      memmove(&(children[i]), &(children[i+1]), (used-i) * sizeof(pivot_child));
      used--;
      return pivot;
    }
    // split's a node, putting the right half of the node in right_n
    // returns the pivot datum (so it can be put in the parent)
    T split(BTreeNode<T,Val_T,C,SIZE> *right_n) {
      size_t pivot_i = (used-1)/2; // middle element for odd "used", lower of 2 middle for even "used"
      memcpy(right_n->data, &(data[pivot_i+1]), (used - pivot_i-1) * sizeof(T));
      memcpy(right_n->children, &(children[pivot_i+1]), (used - pivot_i) * sizeof(right_n));
      right_n->used = used - pivot_i-1;
      used = pivot_i;
      return data[pivot_i];
    }
    // merge's this with right_n, using pivot as the dividing datum
    void merge(T pivot, BTreeNode<T,Val_T,C,SIZE> *right_n) {
      size_t old_used = used;
      used = used + right_n->used + 1;
      set_data(old_used, pivot);
      memcpy(&(data[old_used+1]), right_n->data, right_n->used * sizeof(T));
      memcpy(&(children[old_used+1]), right_n->children, (right_n->used+1) * sizeof(right_n));
    }
};

template<typename T, typename Val_T, typename C, int SIZE>
class BTree {
  static_assert(std::is_same<decltype(C::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on C class");
  static_assert(std::is_same<decltype(C::val(std::declval<T>())), Val_T>(), "Please define a static method Val_T val(T) method on C class");
  private:
    BTreeNode<T,Val_T,C,SIZE> *root;
    Val_T* Check(BTreeNode<T,Val_T,C,SIZE> *n, Val_T *prev);
    bool maybe_split(BTreeNode<T,Val_T,C,SIZE> *parent, BTreeNode<T,Val_T,C,SIZE> *n, size_t i);
    int maybe_merge(BTreeNode<T,Val_T,C,SIZE> *parent, size_t i);
    std::pair<Val_T,Val_T> _check(BTreeNode<T,Val_T,C,SIZE> *n);
    void _print(BTreeNode<T,Val_T,C,SIZE> *n);
  public:
    BTree();
    ~BTree();
    bool get(Val_T val, T* result);
    void insert(T);
    bool remove(Val_T val, T* result);
    void check(void);
    void print(void); 
    bool isempty(void);
};

template<typename T, typename Val_T, typename C, int SIZE>
BTree<T,Val_T,C,SIZE>::BTree() {
  root = nullptr;
}

template<typename T, typename Val_T, typename C, int SIZE>
BTree<T,Val_T,C,SIZE>::~BTree() {
  if (!isempty()) {
    PANIC("Tried to destroy non-empty tree");
  }
}

template<typename T, typename Val_T, typename C, int SIZE>
bool BTree<T,Val_T,C,SIZE>::get(Val_T val, T *result) {
  PRINT("BTree Get, begins\n");
  PRINT_TREE();
  CHECK();
  auto *n = root;
  bool found;
  while(n) {
    size_t i = n->find(val, &found);
    if (found) {
      *result = n->get_data(i);
      PRINT("BTree Get, end found\n");
      PRINT_TREE();
      CHECK();
      return true;
    }
    n = n->get_node(i);
  }
  PRINT("BTree Get, end not found\n");
  PRINT_TREE();
  CHECK();
  return false;
}

template<typename T, typename Val_T, typename C, int SIZE>
bool BTree<T,Val_T,C,SIZE>::isempty(void) {
  // it is possible for root to have only one child
  // it'll resolve as soon as we run a remove or something, but
  // it means we have to check it's child for nullptr
  return root == nullptr || root->get_node(0) == nullptr;
}

template<typename T, typename Val_T, typename C, int SIZE>
void BTree<T,Val_T,C,SIZE>::insert(T datum) {
  PRINT("BTree Insert, begins\n");
  #ifdef BTREE_DEBUG_VERBOSE
  printf("inserting: ");
  C::printT(datum);
  printf("\n");
  #endif
  PRINT_TREE();
  CHECK();
  auto *n = root;
  BTreeNode<T,Val_T,C,SIZE> *parent = nullptr;
  bool found = false;
  size_t i = 0;

  // Root splits look a little different, so seperate them out
  if (root && root->get_used() == SIZE) {
    auto right_n = new BTreeNode<T,Val_T,C,SIZE>();
    T pivot = n->split(right_n);
    auto new_n = new BTreeNode<T,Val_T,C,SIZE>();
    root = new BTreeNode<T,Val_T,C,SIZE>();
    root->set_node(0, n);
    root->insert_right(0, pivot, right_n);
    int c = C::compare(C::val(datum), C::val(root->get_data(i)));
    if (c > 0) { 
      i = 1;
    }
    n = root->get_node(i);
  }

  while(n) {
    i = n->find(C::val(datum), &found);
    if (found) {
      PANIC("Element is already in tree");
    }
    if (maybe_split(n, n->get_node(i), i)) {
      // Rather than find, we can just check this one case
      // Note that when we split, we always add the new node to our right
      int c = C::compare(C::val(datum), C::val(n->get_data(i)));
      if (c > 0) { 
        i++;
      }
      // brute force and ignorance method, solution above is faster
      //i = n->find(C::val(datum), &found);
    }
    parent = n;
    n = n->get_node(i);
  }
  // empty-tree case
  if (!parent) {
    root = new BTreeNode<T,Val_T,C,SIZE>();
    root->insert_right(0, datum, nullptr);
    PRINT("BTree Insert, done\n");
    PRINT_TREE();
    CHECK();
    return;
  }
  // Note - i should always be set by this point
  // because if parent exists, then we had something to search
  // before we looked at n

  // insert only occurs at leafs, parent is a leaf
  // it doesn't matter and insert_right is faster since it avoids any shifting 
  parent->insert_right(i, datum, nullptr);
  PRINT("BTree Insert, done\n");
  PRINT_TREE();
  CHECK();
}

template<typename T, typename Val_T, typename C, int SIZE>
bool BTree<T,Val_T,C,SIZE>::remove(Val_T v, T *result) {
  PRINT("BTree Remove, begins\n");
  PRINT_TREE();
  CHECK();
  BTreeNode<T,Val_T,C,SIZE> *parent = nullptr;
  auto *n = root;
  bool found;
  // if the root node is empty (except one child), delete it
  // this can only happen at root level
  if (n && !n->get_used()) {
    root = n->get_node(0); 
    delete n;
    n = root;
  }
  // find the element to remove
  size_t i;
  while(n) {
    i = n->find(v, &found);
    if (found) {
      // If the node below has to merge (or rotate) then the data we're
      // deleting could migrate when/if we go to get a replacement element.
      // To simplify logic we check for a merge now, and re-search
      // for the element in case it moved
      BTreeNode<T,Val_T,C,SIZE> *left_child = n->get_node(i);
      if (left_child && maybe_merge(n, i)) {
        found = false;
        continue;
      }
      // Now we're safe! the element won't suddenly move on us.
      break; 
    }
    // first merge check is root's child, root can't merge anyway
    //printf("pre-merge i=%ld used=%ld\n", i, n->get_used());
    if (maybe_merge(n, i) == 2) {
      // If maybe_merge returns 1, we're still in the correct node
      // this is the steeling and merge-right cases
      // if we merge left it that node gets deleted, so we need to
      // go to the node it merged with (which is one left)
      i--;
  
      // this is the brute force and ignorance method
      //i = n->find(v, &found);
    }
    n = n->get_node(i);
  }
  // did we find it?
  if (!n) {
    PRINT("BTree Remove, not found\n");
    PRINT_TREE();
    CHECK();
    return false;
  }
  // We found it
  *result = n->get_data(i);
  if (n->get_node(i) == nullptr){
    // If we're a leaf, we can just remove the datum
    BTreeNode<T,Val_T,C,SIZE> *junk;  
    n->remove_right(i, &junk); 
    PRINT("BTree Remove, complete\n");
    PRINT_TREE();
    CHECK();
    return true;
  } 
  // If the node below has to merge then the data we're deleting could migrate
  // down into that node. 

  // If we're an inner node, we have to find a replacement datum
  BTreeNode<T,Val_T,C,SIZE> *r;
  r = n->get_node(i);
  // we've already checked merge on n's left child
  while (r->get_node(r->get_used())) { // walk down the right side of n's left child
    maybe_merge(r, r->get_used());
    r = r->get_node(r->get_used()); 
  }
  BTreeNode<T,Val_T,C,SIZE> *junk;  
  T replacement = r->remove_right(r->get_used()-1, &junk);
  n->set_data(i, replacement); 
  PRINT("BTree Remove, complete\n");
  PRINT_TREE();
  CHECK();
  return true; 
}

template<typename T, typename Val_T, typename C, int SIZE>
bool BTree<T,Val_T,C,SIZE>::maybe_split(BTreeNode<T,Val_T,C,SIZE> *parent, BTreeNode<T,Val_T,C,SIZE> *n, size_t i){
  // We need to always have one spare element, if so, we're good!
  if (!n || n->get_used() < SIZE) {
    return false;
  }
  PRINT("Split begin\n");
  PRINT_TREE();
  CHECK();
  auto right_n = new BTreeNode<T,Val_T,C,SIZE>();
  T pivot = n->split(right_n);
  auto new_n = new BTreeNode<T,Val_T,C,SIZE>();
  parent->insert_right(i, pivot, right_n);
  PRINT("Split end\n");
  PRINT_TREE();
  CHECK();
  return true;  
}

// This returns 0 if nothing changed, nonzero if something did change.
// 1 is returned for events that can only grow parent->get_node(i)
// 2 is returned for events that shrink (actually, delete) parent->get_node(i)
template<typename T, typename Val_T, typename C, int SIZE>
int BTree<T,Val_T,C,SIZE>::maybe_merge(BTreeNode<T,Val_T,C,SIZE> *parent, size_t i){
  PRINT("Maybe merge\n");
  CHECK();
  BTreeNode<T,Val_T,C,SIZE> *n = parent->get_node(i);
  if (!n || n->get_used() > (SIZE-1)/2-1) {
    return 0;
  }
  // now we need to find someone to merge with 
  // check if there's a node to our left
  if (i > 0) {
    auto sibling = parent->get_node(i-1);
    if (sibling->get_used() + n->get_used() >= SIZE) {
      PRINT("stealing from node to left\n");
      // sibling is too large to join with, so rotate instead
      // Rotate right
      BTreeNode<T,Val_T,C,SIZE> *sibling_child;
      T sibling_datum = sibling->remove_right(sibling->get_used()-1, &sibling_child);
      T old_pivot = parent->get_data(i-1); 
      parent->set_data(i-1, sibling_datum); 
      n->insert_left(0, old_pivot, sibling_child);
      PRINT_TREE();
      CHECK();
      return 1;
    }
    PRINT("merging with node to left\n");
    BTreeNode<T,Val_T,C,SIZE> *junk;
    T pivot = parent->remove_right(i-1, &junk); // remove the element left of n, and n
    sibling->merge(pivot, n);
    delete n;
    PRINT_TREE();
    CHECK();
    return 2;
  }
  // if there's nothing to the left, there must be something to the right
  auto sibling = parent->get_node(i+1);
  if (sibling->get_used() + n->get_used() >= SIZE) {
    PRINT("stealing from node to right\n");
    CHECK();
    // sibling is too large to join with, so we rotate instead
    // Rotate left 
    BTreeNode<T,Val_T,C,SIZE> *sibling_child;
    T sibling_datum = sibling->remove_left(0, &sibling_child);
    T old_pivot = parent->get_data(i); 
    parent->set_data(i, sibling_datum); 
    n->insert_right(n->get_used(), old_pivot, sibling_child);
    PRINT_TREE();
    CHECK();
    return 1;
  }
  PRINT("merging with node to right\n");
  BTreeNode<T,Val_T,C,SIZE> *junk;
  T pivot = parent->remove_right(i, &junk); // remove the element right of n, and it's right child
  n->merge(pivot, sibling);
  delete sibling;
  PRINT_TREE();
  CHECK();
  return 1;
}

template<typename T, typename Val_T, typename C, int SIZE>
void BTree<T,Val_T,C,SIZE>::print(void) {
  _print(root);
  printf("\n");
}
 
template<typename T, typename Val_T, typename C, int SIZE>
void BTree<T,Val_T,C,SIZE>::_print(BTreeNode<T,Val_T,C,SIZE> *n) {
  if (!n) {
    printf("n");
    return;
  }
  printf("[");
  size_t i;
  for (i=0; i<n->get_used(); i++) {
    _print(n->get_node(i));
    printf(",");
    // we violate our object interface here... *shrug*
    C::printT(n->get_data(i));
    printf(",");
  }
  _print(n->get_node(n->get_used()));
  printf("]");
}
 
template<typename T, typename Val_T, typename C, int SIZE>
void BTree<T,Val_T,C,SIZE>::check() {
  if (root) {
    _check(root);  
  }
}

template<typename T, typename Val_T, typename C, int SIZE>
std::pair<Val_T,Val_T> BTree<T,Val_T,C,SIZE>::_check(BTreeNode<T,Val_T,C,SIZE> *n) {
  if (n != root && n->get_used() < (SIZE-1)/2-1) {
    printf("Element: ");
    n->print();
    printf("\n");
    PANIC("Element is insufficiently full, and is not root");
  }
  
  // Check ranges
  size_t i;
  int c;
  Val_T min;
  Val_T max;
  bool minmax_initialized=false;
  std::pair<Val_T,Val_T> range;
  bool range_initialized=false;
  std::pair<Val_T,Val_T> oldrange;
  bool oldrange_initialized=false;
  for (i=0; i < n->get_used()+1; i++) {
    if(n->get_node(i)) {
      range = _check(n->get_node(i));
      range_initialized=true;
      if (!minmax_initialized) {
        min = range.first;
        max = range.second;
        minmax_initialized=true;
      } else {
        // track the smallest and largest
        c = C::compare(range.first, min);
        if (c < 0) {
          PANIC("new min is smaller than old min");
          // notionally min=range.first, but we already panic-ed
        }
        c = C::compare(range.second, max);
        if (c < 0) {
          printf("Node: ");
          n->get_node(i)->print();
          printf("\n");
          printf("Child of: ");
          n->print();
          printf("\n");
          PANIC("new max is smaller than old max");
        } else {
          max = range.second;
        }
      }
    }
    if (i != n->get_used()) { 
      Val_T v = C::val(n->get_data(i));
      if (!minmax_initialized){
        min = v;
        max = v;
        minmax_initialized=true;
      }
      // track the smallest and largest
      c = C::compare(v, min);
      if (c < 0) {
        printf("Node: ");
        n->print();
        printf("\n");
        PANIC("new min is smaller than old min");
        // notionally min=range.first, but we already panic-ed
      }
      c = C::compare(v, max);
      if (c < 0) {
        printf("Node: ");
        n->print();
        printf("\n");
        printf("compare result = %d\n", c);
        PANIC("new max is smaller than old max \n");
      } else {
        max = v;
      }
    }
    if (range_initialized) {
      oldrange = range;
      oldrange_initialized=true;
    }
  }
  return std::make_pair(min, max);
}

#endif
