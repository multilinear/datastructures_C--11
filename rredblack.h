#include <stdio.h>
#include <utility>
#include "panic.h"

#ifndef RREDBLACK_H
#define RREDBLACK_H

#define RREDBLACK_DEBUG_VERBOSE

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
    static Node_T *_insert(Node_T *n, Node_T *new_n);
    static Node_T *_balance(Node_T *n);
    static Node_T *_remove(Node_T *n, Val_T v, Node_T **old_n);
    static Node_T *_get_rightmost(Node_T *n, Node_T **rightmost);
  public:
    RRedBlack();
    ~RRedBlack();
    Node_T *get(Val_T v);
    // Returns False if node is already in the tree
    void insert(Node_T *n);
    // Assumes the node is in the tree
    //   if it's not you're going to have a bad time.
    Node_T *remove(Val_T v);
    bool isempty();
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
void RRedBlack<Node_T, Val_T>::insert(Node_T *new_n) {
  PRINT("Begin Insert\n");
  PRINT_TREE();
  CHECK_ALL();
  new_n->right = nullptr;
  new_n->left = nullptr;
  new_n->red = true;
  root = _insert(root, new_n);
  root->red = false;
  PRINT_TREE();
  CHECK_ALL();
}    

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_insert(Node_T *n, Node_T *new_n) {
  if (!n) {
    return new_n;
  }
  int c = Node_T::compare(new_n->val(), n->val());
  if (c > 0) {
    n->right =_insert(n->right, new_n);
  } else if (c < 0) {
    n->left = _insert(n->left, new_n);
  } else {
    PANIC("Element inserted twice!");
  }
  return _balance(n);
}    

template<typename Node_T>
bool isred(Node_T *n) {
  return n && n->red;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_balance(Node_T *n) {
  Node_T *grandparent = n;
  Node_T *a1;  
  Node_T *a2;
  Node_T *a3;
  Node_T *c1;
  Node_T *c2;
  Node_T *c3;
  Node_T *c4;
  //Case 1
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
  } else if (isred<Node_T>(n->right) && isred<Node_T>(n->right->left)) {
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
Node_T *RRedBlack<Node_T, Val_T>::remove(Val_T v) {
  PRINT("Begin Remove\n");
  PRINT_TREE();
  CHECK_ALL();
  Node_T *old_n;
  root = _remove(root, v, &old_n); 
  if (root) {
    root->red = false;
  }
  PRINT_TREE();
  CHECK_ALL();
  return old_n;
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_remove(Node_T *n, Val_T v, Node_T **old_n) {
  if (!n) {
    // Node wasn't found
    *old_n = nullptr;
    return nullptr;
  }
  int c = Node_T::compare(v, n->val());
  if (c > 0) {
    n->right = _remove(n->right, v, old_n);
  } else if (c < 0) {
    n->left = _remove(n->left, v, old_n);
  } else {
    // We found the node to remove
    *old_n = n;
    if (!n->right) {
      // we didn't change n->right
      return n->right; 
    }
    if (!n->left) {
      // we didn't change n->left
      return n->left; 
    }
    if (!n->left->right) {
      n->left->right = n->right;
      return _balance(n->left);
    }
    Node_T *rightmost;
    Node_T *left_tree = _get_rightmost(n->left, &rightmost); 
    rightmost->left = left_tree;
    rightmost->right = n->right;
    rightmost->red = n->red;
    return _balance(rightmost);
  } 
  return _balance(n);
  PANIC("remove not implemented yet"); 
}

template<typename Node_T, typename Val_T>
Node_T *RRedBlack<Node_T, Val_T>::_get_rightmost(Node_T *n, Node_T **rightmost) {
  if (n->right) {
    n->right = _get_rightmost(n->right, rightmost);
    // we pulled a node, so this will need to be replaced
    return _balance(n);
  } 
  *rightmost = n;
  // if rightmost is black, things get interesting
  // We haven't touched n->left, so it doesn't need rebalancing
  return n->left;
}
 
template<typename Node_T, typename Val_T>
void RRedBlack<Node_T, Val_T>::checkAll(void) {
  if (root && root->red) {
    PANIC("root is red!");
  }
  _checkAll(nullptr, root);
  PRINT("check() passed\n");
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
bool RRedBlack<Node_T, Val_T>::isempty(void) {
  return !root;
}

#endif
