/*
 * Copywrite: Matthew Brewer 2014-04-19
 *
 * When to use this:
 * If you want a generic dictionary style structure, btree.h is faster
 * BUT, if your:
 * 1) Need to avoid moving data physically in memory
 * 2) Need "external allocation" meaning you can allocate the memory ourself
 * and put it in to the dict, rather than having it copied.
 * If you need either of these, this is the ideal datastructure for you 
 *
 * For comparitive performance see:
 * PERFORMANCE
 *
 * Design Decisions:
 * 
 *   balance factors:
 * you can track depth or balance factor in an AVL tree. Depth is easier to
 * think about. Given depth though, the data you need to recompute depth on
 * a rotation is not just in the two nodes involved in the rotation, but
 * their children. So you end up accessing another 3 nodes. So you can
 * recompute your depth. Even worse, to figure out whether you need to do
 * a rotation you also need to access those children.
 * With balance factor the computation is much more local, thus saving a
 * lot of extra pointer dereferences
 *
 *   This thing is weird?:
 * When I wrote this implementation I went looking for other AVL
 * implementations, and oddly was unable to find one. Implementations either
 * were not actually AVL trees being actually balanced by size, or more often
 * not actually quite balanced at all, OR they were Log(N)^2 or even NLog(N)
 * for insert and remove due to depth/balance recomputation being idiotic. 
 * So... I derived the formulas for adjusting balance factors myself.
 * The algorithm for adjusting balance as we go up the tree, and terminating
 * early if we are not the deepest branch (or become no longer the deepest branch
 * due to a rotation), is also mine. There may be some other way this is 
 * usually done.
 * 
 * Threadsafety:
 *  thread compatible
 */

#include <stdio.h>
#include <utility>
#include "panic.h"

#ifndef AVL_H
#define AVL_H


// Define this to see tons of detail about what the tree is doing
#ifdef AVL_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#define PRINT_TREE() print();
#else
#define PRINT(msg)
#define PRINT_TREE()
#endif

// Define this to implement some expensive consistancy checking, this is great
// for debugging code that uses the tree as well.
// This checks all of the AVL invariants before and after ever oparation.
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
    Node_T *right;
    Node_T *parent;
    // We shouldn't need the above, just this
    int8_t balance;

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
};

template <typename Node_T, typename Val_T>
class AVL{
  // These ought to be const
  static_assert(std::is_same<decltype(std::declval<Node_T>().val()), Val_T>(), "Please define a method Val_T val() method on Node_T class");
  static_assert(std::is_same<decltype(Node_T::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on Node_T class");
  private:
    Node_T *root;
    void _check(Node_T *parent, Node_T *n) const;
    size_t _checkAll(Node_T *parent, Node_T *n) const;
    void _print(Node_T *n) const;
    int _rotate_left(Node_T *a);
    int _rotate_right(Node_T *a);
  public:
    class Iterator {
      private:
        Node_T *n;
        // This marks whether we've explored right yet, in our iteration
        // this is enough for 64 levels, which would consume all
        // of our virtual address space... should be enough.
        size_t bits;
        size_t level;
      public:
        Iterator(Node_T *nn) {
          n = nn;
          bits = 0;
          level = 0;
					if (n == nullptr) {
						return;
					}
          // Find the left-most branch
          while (n->left) {
            n = n->left;
            level++;
          }
          return;
        }
        Iterator(const Iterator& other) {
          n = other.n;
          bits = other.bits;
          level = other.level;
        }
        Iterator& operator=(const Iterator& other) {
          n = other.n;
          bits = other.bits;
          level = other.level;
          return *this;
        }
        bool operator==(const Iterator& other) const {
          return (n == nullptr && other.n == nullptr) || 
            (n == other.n && bits == other.bits && level == other.level);
        }
        bool operator!=(const Iterator& other) const {
          return !(n == nullptr && other.n == nullptr) && 
            (n != other.n || bits != other.bits || level != other.level);
        }
        Iterator& operator++() {
          // Check if we can go right, and if we have already been right
          if (n->right && !((1<<level) & bits)) {
            // Mark that we've been right from here
            bits |= (1<<level);
            n = n->right;
            level++;
            // new branch, so clear the "have we been right" bit
            bits &= ~(1<<level);
            // And walk all the way down the left
            while (n->left) {
              n = n->left;
              level++;
            }
           return *this;
          }
          // Can't go right, so go up
          n = n->parent;
          // clear the "have we been right" bit in prep for the next descent
          bits &= ~(1<<level);
          level--;
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
    AVL();
    Node_T *get(Val_T v);
    // Returns False if node is already in the tree
    bool insert(Node_T *n);
    // Assumes the node is in the tree
    //   if it's not you're going to have a bad time.
    void remove(Node_T *n);
    bool isempty() const;
    // These are mostly for debugging
    void check(void) const;
    void checkAll(void) const;
    void print(void) const;
};

template <typename Node_T, typename Val_T>
AVL<Node_T, Val_T>::AVL() {
  root = nullptr;
}

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
bool AVL<Node_T, Val_T>::isempty(void) const {
  return !root;
}

/* Transform: 
 *   A         B
 *    \       / 
 *     B  -> A
 *    /       \
 *   C         C
 */
template<typename Node_T, typename Val_T>
int AVL<Node_T, Val_T>::_rotate_left(Node_T *a) {
  Node_T *b = a->right;
  Node_T *c = b->left;
  Node_T *parent = a->parent;
  b->left = a;
  a->parent = b;
  a->right = c;
  if (c) {
    c->parent = a;
  }
  if (parent) {
    if (parent->left == a) {
      parent->left = b;
    } else {
      parent->right = b;
    }
  } else {
    root = b;
  }
  b->parent = parent;
  // and update balance factors
  // YES this is hard to figure out! the result table is terrible.
  // I ran out the entire table, then figured out the rules below from 
  // staring at it. There's probably something better *shrug*
  int8_t a_balance = a->balance;
  int8_t b_balance = b->balance;
  // this is how much deeper the rotation make this subtree.
  // Often negative.
  // In an AVL tree, we should never perform rotations
  // that make it positive.
  int deeper; 
  if (b_balance >= 0) {
    a->balance = a_balance + 1;
    // this case is unneeded, leaving here though
    // because it was so much work to figure out
    //if (a_balance >= 0) {
    //  b->balance = a_balance + b_balance + 2;
    //  deeper = 1;
    //} else {
    b->balance = b_balance + 1;
    deeper = 0;
    //}
  } else {
    a->balance = a_balance - b_balance + 1;
    b->balance = a_balance + 2;
    // for a_balance 1 deeper should actually be 1 not 2
    // but a_balance is always -2 or -1 here, or we're not doing the rotation
    // in an AVL tree
    deeper = a_balance + 1; 
  }
  // returns the amount deeper it made this subtree
  return deeper;
}

/* Transform: 
 *   A    B
 *  /      \ 
 * B   ->   A 
 *  \      /
 *   C    C
 */
template<typename Node_T, typename Val_T>
int AVL<Node_T, Val_T>::_rotate_right(Node_T *a) {
  Node_T *b = a->left;
  Node_T *c = b->right;
  Node_T *parent = a->parent;
  b->right = a;
  a->parent = b;
  a->left = c;
  if (c) {
    c->parent = a;
  }
  if (parent) {
    if (parent->right == a) {
      parent->right = b;
    } else {
      parent->left = b;
    }
  } else {
    root = b;
  }
  b->parent = parent;
  // and update balance factors
  // YES this is hard to figure out! the result table is terrible.
  // I ran out the entire table, then figured out the rules below from 
  // staring at it. There's probably something better *shrug*
  int8_t a_balance = a->balance;
  int8_t b_balance = b->balance;
  int deeper;
  if (b_balance <= 0) {
    a->balance = a_balance - 1;
    // this case is unneeded, leaving here though
    // because it was so much work to figure out
    //if (a_balance <= 0) {
    //  b->balance = a_balance + b_balance - 2;
    //  deeper = 1;
    //} else {
    b->balance = b_balance - 1;
    deeper = 0;
    //}
  } else {
    a->balance = a_balance - b_balance - 1;
    b->balance = a_balance - 2;
    // for a_balance -1 deeper should actually be 1 not 2
    // but a_balance is always 2 or 1 here, or we're not doing the rotation
    // in an AVL tree
    deeper = -a_balance + 1; 
  }
  // returns the amount deeper it made this subtree
  return deeper;
}

template<typename Node_T, typename Val_T>
bool AVL<Node_T, Val_T>::insert(Node_T *n) {
  PRINT("Begin Insert\n");
  CHECK_ALL();
  PRINT_TREE();
  n->right = nullptr;
  n->left = nullptr;
  n->balance = 0;
  // We do this here, so we don't have to do the check every iteration
  if (!root) {
    root = n;
    root->parent = nullptr;
    PRINT_TREE();
    CHECK_ALL();
    PRINT("Insert complete\n");
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
      return false;
    }
  }
  CHECK();
  PRINT("node added\n");
  PRINT_TREE();
  // Now we balance

  // n has no children, so it's balanced.

  // parent didn't have a child on the side n is on (or we wouldn't have added one)
  // thus it either had no children or one child on the other side.
  // Therefore it is either balanced, or only 1 off.

  // So, we start by just checking parent.
  // loop invariant is n and parent are done, we're working on grandparent

  // TODO(mbrewer): we should be able to move this up to where we parented n
  //   saving the duplicate parent->left = n test.
  // fix parent's balance
  if (parent->left == n) {
    parent->balance += 1;
    if (parent->balance <= 0) {
      CHECK_ALL();
      return true;
    }
  } else {
    parent->balance -= 1;
    if (parent->balance >= 0) {
      CHECK_ALL();
      return true;
    }
  }
  // so we need to start at new_n->parent->parent, or grandparent
  // conveniently, parent is the other node we need a handle for.
  Node_T *grandparent = parent->parent;
  while (grandparent) {
    // fix grandparent's balance
    if (grandparent->left == parent) {
      grandparent->balance += 1;
      // if balance is 0 or less, we aren't the deep branch
      // so we haven't added to the depth of grandparent.
      if (grandparent->balance <= 0) {
        break;
      } 
      if (grandparent->balance == 2) {
        if (parent->balance == -1) {
          PRINT("LEFT RIGHT\n");
          PRINT_TREE();
          _rotate_left(parent);
          PRINT_TREE();
          parent = parent->parent; 
        }
        PRINT("LEFT LEFT\n");
        PRINT_TREE();
        // rotate grandparent and parent right 
        int deeper = _rotate_right(grandparent); 
        PRINT_TREE();
        grandparent = parent->parent;
        // if we made this branch shallower, we're done
        if (deeper < 0) {
          break;
        }
        continue;
      }
    } else {
      grandparent->balance -= 1;
      // if balance is 0 or less, we aren't the deep branch
      // so we haven't added to the depth of grandparent.
      if (grandparent->balance >= 0) {
        break;
      }
      if (grandparent->balance == -2) {
        if (parent->balance == 1) {
          PRINT("RIGHT LEFT\n");
          PRINT_TREE();
          _rotate_right(parent);
          PRINT_TREE();
          parent = parent->parent; 
        }
        PRINT("RIGHT RIGHT\n");
        PRINT_TREE();
        int deeper = _rotate_left(grandparent);
        PRINT_TREE();
        grandparent = parent->parent;
        // if we made this branch shallower, we're done
        if (deeper < 0) {
          break;
        }
        continue;
      }
    }
    // go up to the next one
    parent = grandparent;
    grandparent = grandparent->parent;
  }
  PRINT_TREE();
  CHECK_ALL();
  PRINT("Insert complete\n");
  return true;  
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::remove(Node_T *n)  {
  PRINT("Begin remove: ");
  #ifdef AVL_DEBUG_VERBOSE
  n->print();
  printf("\n");
  #endif
  CHECK_ALL();
  PRINT_TREE();

  #ifdef AVL_DEBUG
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
      // if n is the parent, we have to special case things out
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
    int8_t tmp_balance;
    tmp_balance = replacement->balance;
    replacement->balance = n->balance;
    n->balance = tmp_balance;
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
  PRINT("beginning remove balance\n");
  PRINT_TREE();
  // we're going to remove n, but we'll wait until later to simplify logic here.
  Node_T *sibling;
  Node_T *old_n = n;
  while (n->parent) {
    Node_T *parent = n->parent;
    if (parent->left == n) { 
      sibling = parent->right;
      parent->balance -= 1; 
      if (parent->balance == -2) {
        if (sibling->balance == 1) {
          PRINT("RIGHT, LEFT\n");
          PRINT_TREE();
          _rotate_right(sibling);
          PRINT_TREE();
          sibling = sibling->parent; 
        }
        PRINT("RIGHT, RIGHT\n");
        PRINT_TREE();
        int deeper = _rotate_left(parent);
        PRINT_TREE();
        parent = parent->parent;
        // since we're removing, making it shallower
        // doesn't help us
        // and it better not be deeper
        if (deeper == 0) {
          break;
        }
      } else if (parent->balance == -1) {
        break;
      }
    } else {
      sibling = parent->left;
      parent->balance += 1; 
      // we removed an element from n's subtree.
      // so n's sibling is the side with extra elements
      if (parent->balance == 2) {
        if (sibling->balance == -1) {
          PRINT("LEFT, RIGHT\n");
          PRINT_TREE();
          _rotate_left(sibling);
          PRINT_TREE();
          sibling = sibling->parent; 
        }
        PRINT("LEFT, LEFT\n");
        PRINT_TREE();
        int deeper = _rotate_right(parent); 
        PRINT_TREE();
        parent = parent->parent;
        // since we're removing, making it shallower
        // doesn't help us
        // and it better not be deeper
        if (deeper == 0) {
          break; 
        }
      } else if (parent->balance >= 1) {
        break;
      }
    }
    // go up to the next one
    n = parent;
  }
  Node_T *parent = old_n->parent;
  if (parent) {
    if (parent->left == old_n) {
      parent->left = child;
    } else {
      parent->right = child;
    }
  } else {
    root = child;
  }
  if (child) {
    child->parent = parent;
  }
  PRINT_TREE();
  CHECK_ALL();
  PRINT("remove complete\n");
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::check(void) const {
  _check(nullptr, root);
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::_check(Node_T *parent, Node_T *n) const {
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
void AVL<Node_T, Val_T>::checkAll(void) const {
  _checkAll(nullptr, root);
}

template<typename Node_T, typename Val_T>
size_t AVL<Node_T, Val_T>::_checkAll(Node_T *parent, Node_T *n) const {
  if (!n) {
    return 0;
  } 
  size_t size_l = _checkAll(n, n->left);
  size_t size_r = _checkAll(n, n->right);
  // If this is outside of a 32 bit integer, something REALLY
  // exciting happened (it should be -2 to 2... always)
  int diff = size_l-size_r;
  if ((ssize_t) (size_l - size_r) != n->balance) {
    print();
    printf("node's balance is wrong: ");
    n->print();
    printf("\n");   
    PANIC("node doesn't know it's balance");
  }
  if (diff < -1 || diff > 1) {
    print();
    printf("difference between left and right is %d\n", diff);
    printf("for node");
    n->print();
    printf("\n");
    PANIC("left and right subtrees differ in size"); 
  }
  if (n->parent != parent) {
    PRINT_TREE();
    printf("Node: ");
    n->print();
    printf(" Parent: ");
    parent->print();
    printf("\n");
    PANIC("Node is corrupt, it doesn't point to it's parent");
  }
  size_t depth = (size_l > size_r ? size_l : size_r) + 1;
  return depth;
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::print() const {
  _print(root);
  printf("\n");
}

template<typename Node_T, typename Val_T>
void AVL<Node_T, Val_T>::_print(Node_T *n) const {
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
