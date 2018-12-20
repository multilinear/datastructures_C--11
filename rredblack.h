/*
 * Copyright: Matthew Brewer 2014-04-14 
 *
 * When to use this:
 * Don't, avl is superior for the same use cases. Even redblack is a better impl.
 * I'm leaving this code here for comparison/demonstration purposes
 *
 * How to use this library:
 *  You can look at the unittest for an example.
 *  Basically, include this file, write a new "node" class that inherits
 *  from RedBlackNode_base, you'll have to pass the classname into the
 *  template as well.
 *
 *  This class will need a "compare" and a "val" methods. See
 *  RedBlackNode_base for details on these methods.
 *  You may also want to supply a "print" method to aid in debugging
 *  of your class... yeah we should probably sprintf, but I didn't feel like it
 *
 *  In case it's not obvious, you allocate your own memory for use in this
 *  class.
 *
 *  Note that duplicates simply panic this implementation, propogating failure
 *  to insert is a bit awkward without really efficient native pairs. If you
 *  we're to actually use this implementation that would want to be added.
 *  I'm leaving this here for demonstration purposes though, btree or
 *  AVL are better in basically all cases.
 * 
 * Invariants:
 * - Root is always black
 * - Leaves are always black
 * - Red nodes can only have black children
 * - Every path from from any node N to it's descendants has the same number
 *    of black nodes 
 *
 * Should I Use this or redblack.h?:
 *  rredblack uses less storage space, but uses the stack.
 *  redblack uses more storage space, but keeps the stack finite.
 *  redblack runs about 13% faster than rredblack
 *  See PERFORMANCE for more details
 * 
 * Design Decisions:
 *
 * This was made as a test of the comparitive performance of an easy to read
 * version of the code.
 * The design is based loosely on a classic implementation of RedBlack trees
 * in Ocaml. Ocaml uses a write-once store and pattern matching, so the approach
 * here is almost the only way to do it.
 * My hope was that I could make a performant red-black tree that was also legible.
 *
 * ThreadSafety:
 *   thread compatible
 */

#include <stdio.h>
#include <utility>
#include "panic.h"
#include "array.h"

#ifndef RREDBLACK_H
#define RREDBLACK_H

#ifdef RREDBLACK_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#define PRINT_TREE() print();
#else
#define PRINT(msg)
#define PRINT_TREE()
#endif

#ifdef RREDBLACK_DEBUG
#define CHECK() check()
#define CHECK_ALL() checkAll()
#else
#define CHECK()
#define CHECK_ALL()
#endif

// To experiment with the two different balancing algorithms
// and compare performance, define SIMPLER_ALGORITHM
#define SIMPLER_ALGORITHM

// The more complex algorithm outperforms the simple one by about 3%
// in my tests... so pretty small

template<typename Node_T, typename Val_T>
class RRedBlackNode_base {
  public:
    Node_T *left;
    Node_T *right;
    bool red;
};

template<typename Node_T, typename Val_T>
class RRedBlack{
  static_assert(std::is_same<decltype(std::declval<Node_T>().val()), Val_T>(), "Please define a method Val_T val() method on Node_T class");
  static_assert(std::is_same<decltype(Node_T::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on Node_T class");
  private:
    Node_T *root;
    static void _check(Node_T *parent, Node_T *n);
    static size_t _checkAll(Node_T *parent, Node_T *n);
    static void _print(Node_T *n);
    #ifdef SIMPLER_ALGORITHM
    static Node_T *_insert(Node_T *n, Node_T *new_n);
    static Node_T *_insert_balance_right(Node_T *n);
    static Node_T *_insert_balance_left(Node_T *n);
    #else
    static Node_T *_insert(Node_T *n, Node_T *new_n, bool *balanced);
    static Node_T *_insert_balance_right(Node_T *n, bool *balanced);
    static Node_T *_insert_balance_left(Node_T *n, bool *balanced);
    #endif
    static Node_T *_remove(Node_T *n, Val_T v, Node_T **old_n, bool *balanced);
    static Node_T *_remove_balance_right(Node_T *n, bool *balanced);
    static Node_T *_remove_balance_left(Node_T *n, bool *balanced);
    static Node_T *_get_rightmost(Node_T *n, Node_T **rightmost, bool *balanced);
  public:
    class Iterator {
      private:
        UArray<Node_T*> stack;
        Node_T *n;
      public:
        Iterator() {
          n = nullptr;
        }
        Iterator(Node_T *nn) {
          n = nn;
					if (n == nullptr) {
						return;
					}
          while (n->left) {
            stack.push(n);
            n = n->left;
          }
          return;
        }
        Iterator(const Iterator& other) {
          n = other.n;
          array_copy<UArray<Node_T*>,UArray<Node_T*>>(&stack, &(other.stack));
        }
        Iterator& operator=(const Iterator& other) {
          n = other.n;
          array_copy<UArray<Node_T*>,UArray<Node_T*>>(&stack, &(other.stack));
          return *this;
        }
        bool operator==(const Iterator& other) const {
          if (n != other.n) {
            return false;
          }
          if (n == nullptr && other.n == nullptr)  {
            return true;
          }
          size_t i;
          if (stack.size() != other.stack.size()) {
            return false;
          }
          for (i = 0; i < stack.size(); i++) {
            if (stack.get(i) != other.stack.get(i)) {
              return false;
            }
          }
          return true;
        }
        bool operator!=(const Iterator& other) const {
          return !(*this == other);
        }
        Iterator& operator++() {
          if (n->right) {
            // We don't push when we go right
            // 'cause we're done with that node anyway
            n = n->right;
            while (n->left) {
              stack.push(n);
              n = n->left;
            }
           return *this;
          }
          if (!stack.pop(&n)) {
            n = nullptr;
          }
          return *this;
        }
        Iterator operator++(int) {
          Iterator tmp(*this);
          ++(*this);
          return tmp;
        }
        Node_T& operator*() {
          return *n;
        }
        Node_T* operator->() {
          return n;
        }
    };
    Iterator begin() {
      return Iterator(root);
    }
    Iterator end() {
      return Iterator(nullptr);
    }

    RRedBlack();
    ~RRedBlack();
    Node_T *get(Val_T v);
    // Bool here is a lie... Since this code is for demostration, I didn't feel
    // like passing a tuple back (ick in C++), so we just PANIC if it's already there
    bool insert(Node_T *n);
    // RRedblack is weird, we actually gain nothing by knowing the
    // node before doing a deletion, so we break our usual API here
    Node_T *remove(Val_T v);
    bool isempty() const;
    // These are mostly for debugging
    void check(void);
    void checkAll(void);
    void print(void);
};

template<typename Node_T, typename Val_T>
RRedBlack<Node_T, Val_T>::RRedBlack() {
  root = nullptr;
}

template<typename Node_T, typename Val_T>
RRedBlack<Node_T, Val_T>::~RRedBlack() {
  if (root) {
    PANIC("Tree is being destroyed, yet still contains nodes");
  }
}

// Get functions
// This is actually simplier to write non-recursively, and there's no cost
// to doing it this way
template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::get(Val_T v) {
  Node_T *n = root;
  while (true) {
    if (!n){
      return nullptr;
    }
    int c = Node_T::compare(v, n->val());
    if (c > 0) {
      n = n->right;
    } else if (c < 0) {
      n = n->left;
    } else {
      return n;
    }
  }
}

template<typename Node_T, typename Val_T>
bool RRedBlack<Node_T, Val_T>::insert(Node_T *new_n) {
  PRINT("Begin Insert\n");
  PRINT_TREE();
  CHECK_ALL();
  new_n->right = nullptr;
  new_n->left = nullptr;
  new_n->red = true;
  #ifdef SIMPLER_ALGORITHM
  root = _insert(root, new_n);
  #else
  bool balanced = false;
  root = _insert(root, new_n, &balanced);
  #endif
  root->red = false;
  PRINT_TREE();
  CHECK_ALL();
  // TODO: yeah, we should recursively return a tuple
  return true;
}    

template<typename Node_T>
bool isred(Node_T *n) {
  return n && n->red;
}

#ifdef SIMPLER_ALGORITHM
template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert(Node_T *n, Node_T *new_n) {
  if (!n) {
    return new_n;
  }
  int c = Node_T::compare(new_n->val(), n->val());
  if (c > 0) {
    n->right =_insert(n->right, new_n);
    return _insert_balance_right(n);
  } else if (c < 0) {
    n->left = _insert(n->left, new_n);
    return _insert_balance_left(n);
  } else {
    PANIC("Element inserted twice!");
  }
}    

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert_balance_right(Node_T *n) {
  Node_T *a1;  
  Node_T *a2;
  Node_T *a3;
  Node_T *c1;
  Node_T *c2;
  Node_T *c3;
  Node_T *c4;
  // Note: there's an alternative algorithm with 3 cases that colors
  // a2 red more rarely and usually colors it black - this causes less
  // uptree propogation.
  // It's unclear if it would be a win for a recursive version like this.
  if (isred<Node_T>(n->right) && isred<Node_T>(n->right->left)) {
    a1 = n;
    a2 = n->right->left;
    a3 = n->right;
    c1 = n->left;
    c2 = n->right->left->left;
    c3 = n->right->left->right;
    c4 = n->right->right;
  } else if (isred<Node_T>(n->right) && isred<Node_T>(n->right->right)) {
    a1=n;
    a2=n->right;
    a3=n->right->right;
    c1 = n->left;
    c2 = n->right->left;
    c3 = n->right->right->left;
    c4 = n->right->right->right;
  } else {
    return n;
  }
  a2->red = true;
  a2->left = a1;
  a2->right = a3;
  a1->red = false;
  a1->left = c1;
  a1->right = c2;
  a3->red = false;
  a3->left = c3;
  a3->right = c4;
  return a2;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert_balance_left(Node_T *n) {
  Node_T *a1;  
  Node_T *a2;
  Node_T *a3;
  Node_T *c1;
  Node_T *c2;
  Node_T *c3;
  Node_T *c4;
  if (isred<Node_T>(n->left) && isred<Node_T>(n->left->left)) {
    a1 = n->left->left;
    a2 = n->left;
    a3 = n;
    c1 = n->left->left->left;
    c2 = n->left->left->right;
    c3 = n->left->right;
    c4 = n->right;
  } else if (isred<Node_T>(n->left) && isred<Node_T>(n->left->right)) {
    a1 = n->left;
    a2 = n->left->right;
    a3 = n; 
    c1 = n->left->left;
    c2 = n->left->right->left;
    c3 = n->left->right->right;
    c4 = n->right;
  } else {
    return n;
  }
  a2->red = true;
  a2->left = a1;
  a2->right = a3;
  a1->red = false;
  a1->left = c1;
  a1->right = c2;
  a3->red = false;
  a3->left = c3;
  a3->right = c4;
  return a2;
}
#else
template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert(Node_T *n, Node_T *new_n, bool *balanced) {
  if (!n) {
    return new_n;
  }
  int c = Node_T::compare(new_n->val(), n->val());
  if (c > 0) {
    n->right =_insert(n->right, new_n, balanced);
    if (!*balanced) {
      return _insert_balance_right(n, balanced);
    } else {
      return n;
    }
  } else if (c < 0) {
    n->left = _insert(n->left, new_n, balanced);
    if (!*balanced) {
      return _insert_balance_left(n, balanced);
    } else {
      return n;
    }
  } else {
    PANIC("Element inserted twice!");
  }
}    

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert_balance_right(Node_T *n, bool *balanced) {
  PRINT("balance right\n");
  Node_T *grandparent = n;
  Node_T *a1;  
  Node_T *a2;
  Node_T *a3;
  Node_T *c1;
  Node_T *c2;
  Node_T *c3;
  Node_T *c4;
  if (!isred<Node_T>(n->right)) {
    return n;
  } else if (isred<Node_T>(n->left)) {
    PRINT("UNCLE\n");
    n->left->red = false;
    n->right->red =  false;
    n->red = true;
    return n;
  } else if (isred<Node_T>(n->right->left)) {
    PRINT("RIGHT LEFT\n");
    a1 = n;
    a2 = n->right->left;
    a3 = n->right;
    c1 = n->left;
    c2 = n->right->left->left;
    c3 = n->right->left->right;
    c4 = n->right->right;
  } else if (isred<Node_T>(n->right->right)) {
    PRINT("RIGHT RIGHT\n");
    a1=n;
    a2=n->right;
    a3=n->right->right;
    c1 = n->left;
    c2 = n->right->left;
    c3 = n->right->right->left;
    c4 = n->right->right->right;
  } else {
    return n;
  }
  a2->red = false;
  a2->left = a1;
  a2->right = a3;
  a1->red = true;
  a1->left = c1;
  a1->right = c2;
  a3->red = true;
  a3->left = c3;
  a3->right = c4;
  *balanced = true;
  return a2;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert_balance_left(Node_T *n, bool *balanced) {
  PRINT("balance left\n");
  Node_T *grandparent = n;
  Node_T *a1;  
  Node_T *a2;
  Node_T *a3;
  Node_T *c1;
  Node_T *c2;
  Node_T *c3;
  Node_T *c4;
  if (!isred<Node_T>(n->left)) {
    return n;
  } else if (isred<Node_T>(n->right)) {
    PRINT("UNCLE\n");
    n->right->red = false;
    n->left->red =  false;
    n->red = true;
    return n;
  } else if (isred<Node_T>(n->left->right)) {
    PRINT("LEFT RIGHT\n");
    a1 = n;
    a2 = n->left->right;
    a3 = n->left;
    c1 = n->right;
    c2 = n->left->right->right;
    c3 = n->left->right->left;
    c4 = n->left->left;
  } else if (isred<Node_T>(n->left->left)) {
    PRINT("LEFT LEFT\n");
    a1=n;
    a2=n->left;
    a3=n->left->left;
    c1 = n->right;
    c2 = n->left->right;
    c3 = n->left->left->right;
    c4 = n->left->left->left;
  } else {
    return n;
  }
  a2->red = false;
  a2->right = a1;
  a2->left = a3;
  a1->red = true;
  a1->right = c1;
  a1->left = c2;
  a3->red = true;
  a3->right = c3;
  a3->left = c4;
  *balanced = true;
  return a2;
}
#endif


  
template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::remove(Val_T v) {
  PRINT("Begin Remove\n");
  PRINT_TREE();
  CHECK_ALL();
  Node_T *old_n;
  // We could return this recursively from each call, but we don't have easy
  // low overhead pairs.
  // Since we're in a mutable language, we'll use mutation.
  bool balanced = true;
  // remove is called on root, so it already balances things
  root = _remove(root, v, &old_n, &balanced); 
  if (root) {
    root->red = false;
  }
  PRINT_TREE();
  CHECK_ALL();
  return old_n;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_remove(Node_T *n, Val_T v, Node_T **old_n, bool *balanced) {
  if (!n) {
    // Node wasn't found
    *old_n = nullptr;
    return nullptr;
  }
  int c = Node_T::compare(v, n->val());
  if (c > 0) {
    n->right = _remove(n->right, v, old_n, balanced);
    return _remove_balance_right(n, balanced);
  }
  if (c < 0) {
    PRINT("left\n");
    n->left = _remove(n->left, v, old_n, balanced);
    return _remove_balance_left(n, balanced);
  } 
  // We found the node to remove
  *old_n = n;
  if (!n->right) {
    if (!n->red) {
      if (isred(n->left)) {
        n->left->red = false;
      } else {
        *balanced = false;
      }
    }
    // we didn't change n->left
    return n->left; 
  }
  if (!n->left) {
    if (!n->red) {
      if (isred(n->right)) {
        n->right->red = false;
      } else {
        *balanced = false;
      }
    }
    // we didn't change n->right
    return n->right; 
  }
  if (!n->left->right) {
    PRINT("Special left->right case\n");
    Node_T *a = n->left;
    if (!isred(a)) {
      if (isred(a->left)) {
        a->left->red = false;
      } else {
        *balanced = false;
      }
    }
    a->right = n->right;
    a->red = n->red;
    // child that may have changed is to the left
    return _remove_balance_left(a, balanced);
  }
  Node_T *rightmost;
  Node_T *left_tree = _get_rightmost(n->left, &rightmost, balanced); 
  rightmost->left = left_tree;
  rightmost->right = n->right;
  rightmost->red = n->red;
  // child that may have changed is to the left
  return _remove_balance_left(rightmost, balanced);
  
  PANIC("remove not implemented yet"); 
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_get_rightmost(Node_T *n, Node_T **rightmost, bool *balanced) {
  if (n->right) {
    n->right = _get_rightmost(n->right, rightmost, balanced);
    // we pulled a node, so this will need to be replaced
    return _remove_balance_right(n, balanced);
  } 
  // if it's red we can just remove it with no impact
  if (!n->red) {
    // if our child is red, we can just repaint it, still simple
    if (isred(n->left)) {
      n->left->red = false;
    } else {
      // if both n and it's child are black, our "black depth" on this branch is
      // now one to small, so we have to rebalance as we go up.
      *balanced = false;
    }
  }
  *rightmost = n;
  // We haven't touched n->left, so it doesn't need rebalancing
  return n->left;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_remove_balance_left(Node_T *n, bool *balanced) {
  PRINT("_remove_balance_left() called\n");
  Node_T *a1;
  Node_T *a2;
  Node_T *a3;
  Node_T *c3;
  #ifdef RREDBLACK_DEBUG
  if (!n) {
    PANIC("Why did we try and rebalance null?");
  }
  #endif
  // If it's already balanced, do nothing
  if (*balanced) {
    return n;
  }

  // all balance cases require n->right to exist
  if (!n->right) {
    // just propogate upwards
    return n;
  }

  if (!n->red) {
    // case 2, n is black, right sibling is red
    if (n->right->red) {
      a1 = n->left;
      a2 = n;
      a3 = n->right;
      c3 = a3->left;
      a3->left = a2;
      a3->red = false;
      a2->right = c3;
      a2->red = true;
      // this case falls through to 4,5 or 6
      n = a3;
      // we have to recurse on n->left to fall through to 4, 5, or 6
      // n->left is the new "parent"
      n->left = _remove_balance_left(n->left, balanced);
      return n;
    // case 3, n is black, sibling is black, and it's children are black
    } else if (!isred(n->right->left) && !isred(n->right->right)) {
      PRINT("case 3\n");
      n->right->red = true;
      return n;
    }
  } 

  // case 4, n is red, sibling is black, and it's children are black
  if (n->red && !isred(n->right->left) && !isred(n->right->right)) {
    n->red = false;
    n->right->red = true;
    *balanced = true;
    return n;
  }
  
  // case 5, sibling is black, inner child is red, outer child is black 
  if (!n->right->red) {
    if (isred(n->right->left) && !isred(n->right->right)) {
      // we rotate right around n->right
      a1 = n->right->left;
      a2 = n->right;
      a2->left = a1->right;
      a1->right = a2;
      a1->red = false;
      a2->red = true;
      // this case falls through to 6
      n->right = a1;
    } 
    // no else here, we need fallthrough
    // case 6, sibling is black, outer child is red
    if (!n->right->red && isred(n->right->right)) {
      a2 = n;
      a3 = n->right;
      a2->right = a3->left;
      a3->left = a2;
      a3->right->red = false;
      a3->red = a2->red;
      a2->red = false;
      *balanced = true;
      return a3;
    }
  }
  // Nothing matched... keep propogating!
  return n;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_remove_balance_right(Node_T *n, bool *balanced) {
  Node_T *a1;
  Node_T *a2;
  Node_T *a3;
  Node_T *c3;
  #ifdef RREDBLACK_DEBUG
  if (!n) {
    PANIC("Why did we try and rebalance null?");
  }
  #endif
  // If it's already balanced, do nothing
  if (*balanced) {
    return n;
  }

  // all balance cases require n->left to exist
  if (!n->left) {
    // just propogate upwards
    return n;
  }

  if (!n->red) {
    // case 2, n is black, left sibling is red
    if (n->left->red) {
      a1 = n->right;
      a2 = n;
      a3 = n->left;
      c3 = a3->right;
      a3->right = a2;
      a3->red = false;
      a2->left = c3;
      a2->red = true;
      // this case falls through to 4,5 or 6
      n = a3;
      // we have to recurse on n->right to fall through to 4, 5, or 6
      // n->right is the new "parent"
      n->right = _remove_balance_right(n->right, balanced);
      return n;
    // case 3, n is black, sibling is black, and it's children are black
    } else if (!isred(n->left->right) && !isred(n->left->left)) {
      n->left->red = true;
      return n;
    }
  } 
  // We don't use an else here, because we need fallthrough from case 2
  // case 4, n is red, sibling is black, and it's children are black
  if (n->red && !isred(n->left->right) && !isred(n->left->left)) {
    n->red = false;
    n->left->red = true;
    *balanced = true;
    return n;
  }

  // case 5, sibling is black, inner child is red, outer child is black 
  if (!n->left->red) {
    if (isred(n->left->right) && !isred(n->left->left)) {
      // we rotate left around n->left
      a1 = n->left->right;
      a2 = n->left;
      a2->right = a1->left;
      a1->left = a2;
      a1->red = false;
      a2->red = true;
      // this case falls through to 6
      n->left = a1;
    } 
    // no else here, we need fallthrough
    // case 6, sibling is black, outer child is red
    if (!n->left->red && isred(n->left->left)) {
      a2 = n;
      a3 = n->left;
      a2->left = a3->right;
      a3->right = a2;
      a3->left->red = false;
      a3->red = a2->red;
      a2->red = false;
      *balanced = true;
      return a3;
    }
  }
  // Nothing matched... keep propogating!
  return n;
}

template<typename Node_T, typename Val_T>
void RRedBlack<Node_T, Val_T>::checkAll(void) {
  if (root && root->red) {
    PANIC("root is red!");
  }
  _checkAll(nullptr, root);
}


template<typename Node_T, typename Val_T>
size_t RRedBlack<Node_T, Val_T>::_checkAll(Node_T *parent, Node_T *n) {
  if (!n) {
    return 1;
  }
  size_t b_left = _checkAll(n, n->left);
  size_t b_right = _checkAll(n, n->right);
  if (b_left != b_right) {
    PANIC("left subtree has different black count than right subree");
  }
  if (n->red) {
    // red nodes must have only black children
    if (parent && parent->red && n->red) {
      PANIC("Red node has red children\n");
    }
  } else {
    b_left += 1;
  }
  return b_left;    
}

template<typename Node_T, typename Val_T>
void RRedBlack<Node_T, Val_T>::print() {
  _print(root);
  printf("\n");
}

template<typename Node_T, typename Val_T>
void RRedBlack<Node_T, Val_T>::_print(Node_T *n) {
  if (!n) {
    printf("n");
    return;
  }
  printf("(");
  n->print();
  printf("|");
  _print(n->left);
  _print(n->right);
  printf(")");
}

template<typename Node_T, typename Val_T>
bool RRedBlack<Node_T, Val_T>::isempty(void) const {
  return !root;
}

#endif
