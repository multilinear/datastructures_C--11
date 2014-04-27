/*
 * Copywrite: Matthew Brewer 2014-04-19
 *
 */

#include <stdio.h>
#include <panic.h>

#ifndef AVL_H
#define AVL_H

#ifdef AVL_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#define PRINT_TREE() print();
#else
#define PRINT(msg)
#define PRINT_TREE()
#endif

#ifdef AVL_DEBUG
#define CHECK() check()
#define CHECK_ALL() checkAll()
#else
#define CHECK()
#define CHECK_ALL()
#endif


template<typename Node_T, typename Val_T>
class AVLNode_base {
  public:
    Node_T *left;
    Node_T *right
    Node_T *parent;
    // TODO(mbrewer): remove this!
    // This gives 2^256 nodes in the tree... that's a LOT
    uint8_t depth;
    // We shouldn't need the above, just this
    uint8_t balance;

    // The following functions need to be overriden!
    // They do not need to be virtual though, so simply don't exist in the base
    // class

    // Note: these don't need to be virtual, because we template on Node_T
    // const Val_T val(void) = 0;

    // Note: sign extension takes care of cast if you use subtraction
    // This is equivelent to v1 - v2
    //static int compare(const Val_T *v1, const Val_T *v2) = 0; 

    // This can be overriden to help with debugging, no need to make it virtual
    void print(void) {
      printf("?");
    }
    // This should not be overriden
    int balance_factor() {
      int depth_l = left ? left->depth : 0;
      int depth_r = right? right->depth: 0;
      return depth_l - depth_r;
    }
    void recompute_depth() {
      int depth_l = left ? left->depth : 0;
      int depth_r = right? right->depth: 0;
      depth = (depth_l > depth_r ? depth_l : depth_r) + 1;
    }
}

template <typename Node_T, typename Val_T>
class AVL{
  static_assert(std::is_same<decltype(std::declval<Node_T>().val()), Val_T>(), "Please define a method Val_T val() method on Node_T class");
  static_assert(std::is_same<decltype(Node_T::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on Node_T class");
  private:
    Node_T *root;
    void _check(Node_T *parent, Node_T *n);
    size_t _checkAll(Node_T *parent, Node_T *n);
    void _print(Node_T *n);
  public:
    AVL();
    Node_T *get(Val_T v);
    // Returns False if node is already in the tree
    bool insert(Node_T *n);
    // Assumes the node is in the tree
    //   if it's not you're going to have a bad time.
    Node_T* remove(Node_T *n);
    bool isempty();
    // These are mostly for debugging
    void check(void);
    void checkAll(void);
    void print(void);
}

template <typename Node_T, typename Val_T>
AVL<Node_T, Val_T>::AVL() {
  root = nullptr;
}

// Get functions
template<typename Node_T, typename Val_T>
Node_T *AVL<Node_T, Val_T>::get(Val_T v) {
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
bool AVL<Node_T, Val_T>::isempty(void) {
  return !root;
}

template<typename Node_T, typename Val_T>
bool AVL<Node_T, Val_T>::insert(Node_T *n) {
  PRINT("Begin Insert\n");
  CHECK_ALL();
  PRINT_TREE();
  n->right = nullptr;
  n->left = nullptr;
  n->depth = 1;
  n->balance = 0;
  // We do this here, so we don't have to do the check every iteration
  if (!root) {
    root = n;
    root->parent = nullptr;
    root->red = false; // root so recolor black
    CHECK_ALL();
    return true;
  }
  Node_T *parent = root;

  // insert it
  while (true) {
    int c = Node_T::compare(n->val(), parent->val());
    if (c > 0) {
      if (!parent->right) {
        parent->right = n;
        n->parent = parent;
        break;
      }
      parent = parent->right;
    } else if (c < 0) {
      if (!parent->left) {
        parent->left = n;
        n->parent = parent;
        break;
      }
      parent = parent->left;
    } else {
      // We already have one of those
      CHECK_ALL();
      return false;
    }
  }
  CHECK();
  // Now we balance

  // n has no children, so it's balanced.

  // it's parent has depth 1 on one side, and it was nearly balanced before
  // so it's balanced enough

  // so we need to start at new_n->parent->parent, or grandparent
  // conveniently, parent and new_n are the other nodes we need handles for.
  Node_T *grandparent = parent->parent;
  Node_T* tmp;
  // tracks whether we've found a root for which we aren't a new deeper branch
  // once we do the balance stops propogating
  bool balance_propogate = true;
  // fix parent's balance
  if (parent->left == n) {
    if (parent->balance <= 0) {
      balance_propogate = false; 
    }
    parent->balance += 1;
  } else {
    if (parent->balance >= 0) {
      balance_propogate = false; 
    }
    parent->balance -= 1;
  }
  while (grandparent && balance_propogate ) {
    // fix grandparent's balance
    if (grandparent->left == n) {
      if (grandparent->balance <= 0) {
        balance_propogate = false; 
      }
      grandparent->balance += 1;
    } else {
      if (grandparent->balance >= 0) {
        balance_propogate = false; 
      }
      grandparent->balance -= 1;
    }
    // We have to correct depths as we go up the tree, so we can
    // compute balance factors.
    // balance factors only depend on children's depth, so we only need
    // to make sure n and parent's depths' get updated each pass
    // n is already correct, so we update parent
    parent->recompute_depth();
  
    if (grandparent->balance_factor() == 2) {
      if (parent->balance_factor() == -1) {
        // make n's left child parent's right child
        parent->right = n->left;
        if(parent->right) {
          parent->right->parent = parent;
        }
        // make parent n's left child
        n->left = parent;
        parent->parent = n;
        // and make n grandfather's left child
        grandfather->left = n;
        n->parent = grandfather;
        // and reset parent and n pointers
        tmp = n;
        n = parent;
        parent = tmp; 
        // we moved these nodes
        n->recompute_depth();
        parent->recompute_depth();
      }
      // rotate grandparent and parent right 
      Node_T * great_grandparent = grandparent->parent;
      // make parent's right child grandparent's left child
      grandparent->left = parent->right;
      if(grandparent->left) {
        grandparent->left->parent = grandparent;
      }
      // make grandparent parent's right child
      parent->right = grandparent;
      grandparent->parent = parent;
      // and make parent great_grandparent's child
      if (great_grandparent) {
        if (great_grandparent->left = grandparent) {
          great_grandparent->left = parent;
        } else {
          great_grandparent->right = parent;
        parent->parent = great_grandparent;
      } else {
        root = parent;
        parent->parent = nullptr;
      }
      // we moved grandparent down a new subtree under it
      // so we need to recompute it's depth
      grandparent->recompute_depth();
      // parent now has grandparent for a child, so we need to recpmute it's depth
      parent->recompute_depth();
      // and finally set up for our next iteration
      grandparent = parent;
      parent = n;
    } else if (grandparent->balance_factor() == -2) {
      if (parent->balance_factor() == 1) {
        // make n's right child parent's left child
        parent->left = n->right;
        if(parent->left) {
          parent->left->parent = parent;
        }
        // make parent n's right child
        n->right = parent;
        parent->parent = n;
        // and make n grandfather's right child
        grandfather->right = n;
        n->parent = grandfather;
        // and reset parent and n pointers
        tmp = n;
        n = parent;
        parent = tmp; 
        // we moved these nodes
        n->recompute_depth();
        parent->recompute_depth();
      }
      // rotate grandparent and parent left 
      Node_T * great_grandparent = grandparent->parent;
      // make parent's left child grandparent's right child
      grandparent->right = parent->left;
      if(grandparent->right) {
        grandparent->right->parent = grandparent;
      }
      // make grandparent parent's left child
      parent->left = grandparent;
      grandparent->parent = parent;
      // and make parent great_grandparent's child
      if (great_grandparent) {
        if (great_grandparent->right = grandparent) {
          great_grandparent->right = parent;
        } else {
          great_grandparent->left = parent;
        parent->parent = great_grandparent;
      } else {
        root = parent;
        parent->parent = nullptr;
      }
      // we moved grandparent down a new subtree under it
      // so we need to recompute it's depth
      grandparent->recompute_depth();
      // parent now has grandparent for a child, so we need to recpmute it's depth
      parent->recompute_depth();
      // and finally set up for our next iteration
      grandparent = parent;
      parent = n;
    }
    // go up to the next one
    n = parent; 
    parent = grandparent;
    grandparent = grandparent->parent;
    // note that parent's depth was correct, so now n's depth is correct.
    // as it was when we entered the loop
  }
  // On the last iteration grandparent is root, we walk up to where grandparent
  // is null, and then drop out of the loop. thus parent is now root, but it's
  // depth hasn't been calculated, just for cleanliness, so all depths of all
  // nodes are known we do it here.
  // Note that this could be elided (if you do, update the checkAll() method).
  parent->recompute_depth();
  CHECK_ALL();
  return true;  
}

template<typename Node_T, typename Val_T>
Node_T *AVL<Node_T, Val_T>::remove(Node_T *n)  {
  CHECK_ALL();
  PRINT_TREE();

  #ifdef REDBLACK_DEBUG
  if (!n) {
    PANIC("Tried to remove null");
  }
  if (!get(n->val())) {
    PANIC("Tried to remove node from tree it isn't in");
  } 
  #endif

  // Swap nodes so we're deleting something with only one child
  Node_T *child;
  if (n->right) {
    Node_T *replacement;
    Node_T *tmp;
    PRINT("finding replacement\n");
    // CAREFUL: we use the fact that we went right then left later
    // if you decide to go left then right there's a lot of code to change.
    //
    // we grab the leftmost node from the right subtree
    replacement = n->right;
    while (replacement->left) {
      replacement = replacement->left;
    }
    // NOTE: do NOT just swap contents! even if you can figure out how
    //   We do not control memory allocation, and we have no idea if
    //   someone else has a pointer to that node. we have to move
    //   metadata instead.
    Node_T *parent = n->parent;
    if (replacement->parent != n) {
      // If n is not the parent, we swap all state
      PRINT("n is not replacement's parent case\n");

      // Replacement is not parent's child
      //   therefore it's a left child 
      //   Thus we can elide the check for which child replacement is
      // make n, replacement's parent's child
      replacement->parent->left = n;
      n->parent = replacement->parent;

      tmp = replacement->right;
      // make n's right child, replacement's right child 
      replacement->right = n->right;
      if (replacement->right) {
        replacement->right->parent = replacement;
      }
      // make replacement's right child, n's right child
      n->right = tmp;
      if(n->right) {
        n->right->parent = n;
      }
    } else {
            // if n is the parent, we have to do special case things out
      PRINT("Case: n is replacement's parent\n");
      // known: replacement can only be n's right child
      //   because we took one step right, then walked left
      // make replacement's right child, n's right child
      n->right = replacement->right;
      if (n->right) {
        n->right->parent = n;
      }
      // make n, replacement's right child
      replacement->right = n;
      n->parent = replacement;

    }
    // make n's left child, replacement's left child
    replacement->left = n->left;
    if (replacement->left) {
      replacement->left->parent = replacement;
    }
    // We know replacement doesn't have a left child
    //  Thus we can elide the null check and reparent
    // make replacement's left child, n's left child
    n->left = nullptr;
    // and make replacement, parent's child
    if (parent) {
      if (parent->left == n) {
        parent->left = replacement;
      } else {
        parent->right = replacement;
      }
    } else {
      root = replacement;
    }
    replacement->parent = parent;
    // swap color too
    uint8_t tmp_depth;
    tmp_depth = replacement->depth;
    replacement->depth = n->depth;
    n->depth = tmp_depth;
    // and continue with removing n, now in a more useful place
    PRINT("Done replacement\n");
    PRINT_TREE();
    // our new n can only have a right child
    child = n->right;
  } else {
    // if we don't have a right child we use the left one
    child = n->left;
  }
  // If there's no right subtree than n is fine to delete already
  // Nodeswap is complete
  CHECK();

    


  CHECK_ALL();
  return nullptr;
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::check(void) {
  _check(nullptr, root);
  PRINT("check() passed\n");
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::_check(Node_T *parent, Node_T *n) {
  if (!n) {
    return;
  }
  _check(n, n->left);
  if (n->parent != parent) {
    printf("Node: ");
    n->print();
    printf(" Parent: ");
    parent->print();
    printf("\n");
    PANIC("Node is corrupt, it doesn't point to it's parent");
  }
  _check(n, n->right);
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::checkAll(void) {
  checkALL(nullptr, root);
  PRINT("check() passed\n");
}

template<typename Node_T, typename Val_T>
int AVL<Node_T, Val_T>::_checkAll(Node_T *parent, Node_T *n) {
  if (!n) {
    return 0;
  } 
  int size_l = checkAll(n, n->left);
  int size_r = checkAll(n, n->right);
  int diff = size_l-size_r;
  if (diff < -1 || diff > 1) {
    PANIC("left and right subtrees differ in size"); 
  }
  if (n->parent != parent) {
    printf("Node: ");
    n->print();
    printf(" Parent: ");
    parent->print();
    printf("\n");
    PANIC("Node is corrupt, it doesn't point to it's parent");
  }
  int depth = (size_l > size_r ? size_l : size_r) + 1;
  if (depth != n->depth) {
    PANIC("node doesn't know it's depth");
  }
  return depth;
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::print() {
  _print(root);
  printf("\n");
}

template<typename Node_T, typename Val_T>
void RedBlack<Node_T, Val_T>::_print(Node_T *n) {
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


#endif
