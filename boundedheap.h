/* Copyright 2014-05-20: Matthew Brewer
 *
 * This is a Minheap, implemented as a Tree.
 *
 * When to use this:
 * Most likely don't.
 * The exception is if you you want external allocation (you allocate memory,
 * and not the algorithm)
 * It's slow, it gives tight bounds on worst-case runtime, but heap with a delayed copy array
 * is faster with the same bounds.
 *
 * A tree implementation of a heap is undoubtably slower than an array
 * implementation. This was implemented to find out how much slower.
 *
 * A tree implementation also has one major advantage. In an array
 * implementation, if you outstrip your array size, you have to
 * double the size of the array. This is an O(N) operation on push
 * which should ostensibly be O(log(N)). (Though again, delayed copy array
 * as a backing store for heap solves this).
 *
 * With a tree implementation all operations are O(log(N))... always.
 * This is why I call it a "Bounded Heap" because it has a tighter
 * bound on the maximum runtime than a standard heap does.
 *
 * Threadsafety:
 *   Thread compatible
 */

#include <cstring>
#include <stdio.h>
#include <utility>
#include "panic.h"

#ifndef BOUNDEDHEAP_H
#define BOUNDEDHEAP_H

//#define BOUNDEDHEAP_DEBUG_VERBOSE

// Define this to see tons of detail about what the tree is doing
#ifdef BOUNDEDHEAP_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#define PRINT_HEAP() print();
#else
#define PRINT(msg)
#define PRINT_HEAP()
#endif

// Define this to implement some expensive consistancy checking, this is great
// for debugging code that uses the tree as well.
// This checks all of the heap invariants before and after ever oparation.
#ifdef BOUNDEDHEAP_DEBUG
#define CHECK() check()
#define CHECK_ALL() check()
#else
#define CHECK()
#define CHECK_ALL()
#endif


template <typename Node_T, typename Val_T>
class BoundedHeapNode_base {
  public:
    Node_T *left;
    Node_T *right;
    Node_T *parent;
    void print(void) {
      printf("?");
    }
};

template <typename Node_T, typename Val_T>
class BoundedHeap {
  private:
    Node_T *root;   
    Node_T *tail;
    bool going_left;
    Node_T **get_next_tail(Node_T **parent);
    Node_T *get_prev_tail(void);
    void unlink(Node_T *n);
    void swap_with_parent(Node_T *n);
    void bubble_up(Node_T *n);
    void bubble_down();
    void _check(Node_T *n, Node_T *parent, size_t depth, size_t *deepest, size_t *shallowest);
    void _check_all(Node_T *n, Node_T *parent, size_t depth, size_t *deepest, size_t *shallowest);
    void _print(Node_T *n);
  public:
    BoundedHeap();
    void push(Node_T *n);
    Node_T *pop(void);
    Node_T *peek(void);
    void check(void);
    void check_all(void);
    void print(void);
};

template<typename Node_T, typename Val_T>
BoundedHeap<Node_T,Val_T>::BoundedHeap() {
  root = nullptr;
  going_left = false;
}

template<typename Node_T, typename Val_T>
Node_T **BoundedHeap<Node_T,Val_T>::get_next_tail(Node_T **new_parent) {
  Node_T *n;
  n = tail;
  Node_T *parent = n->parent;
  if (going_left) {
    // going left
    while(parent && parent->left == n) {
      n = parent;
      parent = parent->parent;
    }
    // We broke out of the loop either because n is parent's right child
    // or because parent doesn't exist

    // if parent doesn't exist, then n is root. we failed to find a branch to
    // our left, so extend the tree to a new level and start going the other way
    if (!parent) {
      going_left = false;
      *new_parent = tail;
      return &(tail->left);
    }
    // we found a parent where n is it's right child, so go left
    n = parent->left;
    // if left doesn't exist, this is where we should put the next element
    if (!n) {
      *new_parent = parent;
      return &(parent->left);
    }
    // if it does exist, go right as far as we can
    while(n->right) {
      n = n->right;
    }
    // and put the node there
    *new_parent = n;
    return &(n->right);
  }
  // going right
  while(parent && parent->right == n) {
    n = parent;
    parent = parent->parent;
  }
  // if we hit the top, tail is already all the way left
  if (!parent) {
    going_left = true;
    *new_parent = tail;
    return &(tail->right);
  }
  n = parent->right;
  if (!n) {
    *new_parent = parent;
    return &(parent->right);
  }
  while(n->left) {
    n = n->left;
  }
  *new_parent = n;
  return &(n->left);
}

template<typename Node_T, typename Val_T>
Node_T *BoundedHeap<Node_T,Val_T>::get_prev_tail(void) {
  Node_T *n;
  n = tail;
  Node_T *parent = n->parent;
  if (going_left) {
    // going left
    while(parent && parent->right == n) {
      n = parent;
      parent = parent->parent;
    }
    // either parent doesn't exist
    // or n is parent's right child

    // parent doesn't exist then n is root
    // this means tail was not in root's left subtree
    if (!parent) {
      going_left = false;
      return tail->parent;
    }
    
    n = parent->right;
    while(n->left) {
      n = n->left;
    }
    return n;
  }
  // going right
  while(parent && parent->left == n) {
    n = parent;
    parent = parent->parent;
  }
  // if we hit the top, tail is all the way left
  if (!parent) {
    going_left = true;
    return tail->parent;
  }
  n = parent->left;
  while(n->right) {
    n = n->right;
  }
  return n;
}

// only usable on leaf nodes
template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::unlink(Node_T *n) {
  if(n->parent->left == n) {
    n->parent->left = nullptr;
  } else {
    n->parent->right = nullptr;
  }
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::swap_with_parent(Node_T *n) {
  PRINT_HEAP();
  CHECK();
  Node_T *parent = n->parent;
  Node_T *grandparent = parent->parent;
  // swap the children
  Node_T *tmp_left = n->left;
  Node_T *tmp_right = n->right;
  if(n == parent->left) {
    // make parent n's left child
    n->left = parent;
    parent->parent = n;
    // make parent->right n's right child
    n->right = parent->right;
    if (n->right) {
      n->right->parent = n;
    }
  } else {
    // make parent n's right child
    n->right = parent;
    parent->parent = n;
    // make parent->left n's left child
    n->left = parent->left;
    if (n->left) {
      n->left->parent = n;
    }
  } 
  // make n's old left child parent's left child
  parent->left = tmp_left;
  if (tmp_left) {
    tmp_left->parent = parent;
  }
  // make n's old right child parent's right child
  parent->right = tmp_right;
  if (tmp_right) {
    tmp_right->parent = parent;
  }
  // lastly make n grandparent's child (or fix root)
  n->parent = grandparent;
  if (grandparent) {
    if(grandparent->left == parent) {
      grandparent->left = n;
    } else {
      grandparent->right = n;
    }
  } else {
    root = n;
  }

  // TODO(mbrewer): could probably avoid this check for *every* swap
  if (tail == n) {
    tail = parent;
  }
  CHECK();
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::bubble_up(Node_T *n) {
  while (n->parent) {
    int c = Node_T::compare(n->val(), n->parent->val());
    if (c >= 0) {
      break;
    }
    // if n is smaller, tree is out of order, so swap
    swap_with_parent(n);
  }
}
      
template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::bubble_down() {
  int c = 0;
  Node_T *n = root;
  while (true) {
    CHECK();
    // Pick the node that is smaller, and exists
    if (n->left && n->right) {
      c = Node_T::compare(n->left->val(), n->right->val());
    }
    if (n->left && (!n->right || c<0)) {
      // left is smaller (or right doesn't exist)
      c = Node_T::compare(n->val(), n->left->val());
      // n is larger than it's child, tree is out of order, so swap
      if (c >= 0) {
        swap_with_parent(n->left);
      } else {
        return;
      }
    } else if (n->right) {
      // right is smaller, or they are equal, or left doesn't exist
      c = Node_T::compare(n->val(), n->right->val());
      // n is larger than it's child, tree is out of order, so swap
      if (c >= 0) {
        swap_with_parent(n->right);
      } else {
        return;
      }
    } else {
      // neither left or right exists, so we're at the bottom of the tree
      return;
    }
  }
}     
    
template<typename Node_T, typename Val_T>
Node_T *BoundedHeap<Node_T,Val_T>::peek(void) {
  return root;
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::push(Node_T *n) {
  PRINT("*** Push\n");
  PRINT_HEAP();
  CHECK_ALL();
  n->left = nullptr;
  n->right = nullptr;
  n->parent = nullptr;
  if (!root) {
    root = n;
    tail = n;
  } else{
    Node_T *parent = nullptr;
    *(get_next_tail(&parent)) = n;
    n->parent = parent;
    tail = n;
    PRINT_HEAP();
    bubble_up(n);
  }
  PRINT("*** Push complete\n");
  CHECK_ALL();
  PRINT_HEAP();
}

template<typename Node_T, typename Val_T>
Node_T* BoundedHeap<Node_T,Val_T>::pop() {
  PRINT("*** Pop\n");
  CHECK_ALL();
  PRINT_HEAP();
  if (!root) {
    return nullptr;
  }
  Node_T *old_root = root;
  Node_T *old_tail = tail;
  if (root == tail) {
    root = nullptr;
    tail = nullptr;
    return old_tail;
  }
  tail = get_prev_tail();
  unlink(old_tail);
  old_tail->left = root->left;
  old_tail->right = root->right;
  old_tail->parent = nullptr;
  if (old_tail->left) {
    old_tail->left->parent = old_tail;
  }
  if (old_tail->right) {
    old_tail->right->parent = old_tail;
  }
  // In the case where tail is now the root, we're removing root
  // so we need tail to be the new root instead.
  if (tail == root) {
    tail = old_tail;
  }
  root = old_tail;
  bubble_down();
  PRINT("*** Pop complete\n");
  CHECK_ALL();
  PRINT_HEAP();
  return old_root;
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::print() {
  _print(root);
  printf("\n");
}
 
template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::_print(Node_T *n) {
  if (!n) {
    printf("n");
    return;
  }
  printf("[");
  n->print();
  printf(":");
  _print(n->left);
  printf(",");
  _print(n->right);
  printf("]");
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::check() {
  size_t deepest=0;
  size_t shallowest=9999; // this is *actually* big enough, unless you plan to fill more than main memory
  _check(root, nullptr, 0, &deepest, &shallowest); 
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::check_all() {
  size_t deepest=0;
  size_t shallowest=9999; // this is *actually* big enough, unless you plan to fill more than main memory
  _check_all(root, nullptr, 0, &deepest, &shallowest); 
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::_check(Node_T *n, Node_T *parent, size_t depth, size_t *deepest, size_t *shallowest) {
  if (!n) {
    if (depth > *deepest) {
      *deepest = depth;
    }
    if (depth < *shallowest) {
      *shallowest = depth;
    }
    if (*shallowest != *deepest && *shallowest+1 != *deepest) {
      printf("shallowest branch is depth %ld, deepest is depth %ld\n", *shallowest, *deepest);
      PANIC("Tree is not complete\n");
    }
    return;
  }
  // Did we link to ourselves?
  if(n == parent || n->parent == n) {
    PANIC("parent is US!\n");
  }
  // is our tree consistant
  if(n->parent != parent) {
    printf("%ld != %ld\n", (long) (n->parent), (long) parent);
    PANIC("parent pointer is corrupt");
  }
  _check(n->left, n, depth+1, deepest, shallowest);
  _check(n->right, n, depth+1, deepest, shallowest);
  // is the heap "complete"?
}

template<typename Node_T, typename Val_T>
void BoundedHeap<Node_T,Val_T>::_check_all(Node_T *n, Node_T *parent, size_t depth, size_t *deepest, size_t *shallowest) {
  if (!n) {
    if (depth > *deepest) {
      *deepest = depth;
    }
    if (depth < *shallowest) {
      *shallowest = depth;
    }
    if (*shallowest != *deepest && *shallowest+1 != *deepest) {
      printf("shallowest branch is depth %ld, deepest is depth %ld\n", *shallowest, *deepest);
      PANIC("Tree is not complete\n");
    }
    return;
  }
  // Did we link to ourselves?
  if(n == parent || n->parent == n) {
    PANIC("parent is US!\n");
  }
  // is our tree consistant
  if(n->parent != parent) {
    printf("%ld != %ld\n", (long) (n->parent), (long) parent);
    PANIC("parent pointer is corrupt");
  }
  // basic heap invariants
  int c;
  if (n->left) {
    c = Node_T::compare(n->val(), n->left->val());
    if (c > 0) {
      PANIC("Node is larger than it's child\n"); 
    }
  }
  if (n->right) {
    c = Node_T::compare(n->val(), n->right->val());
    if (c > 0) {
      PANIC("Node is larger than it's child\n"); 
    }
  }
  _check_all(n->left, n, depth+1, deepest, shallowest);
  _check_all(n->right, n, depth+1, deepest, shallowest);
  // is the heap "complete"?


}

#endif
