/*
 * Copyright: Matthew Brewer 2014-04-14 
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
 *  In case it's not obvious, you allocate you're own memory for use in this
 *  class.
 * 
 * Invariants:
 * - Root is always black
 * - Leaves are always black
 * - Red nodes can only have black children
 * - Every path from from any node N to it's descendants has the same number
 *    of black nodes 
 *
 * For comparitive performance of structures see
 * PERFORMANCE
 *
 * Design Decisions:
 *
 *   We use no recursion (except traversals):
 * Really, it doesn't much matter, but it was just as easy to write it
 * non-recursively. Most likely this won't hurt the compiler, and it
 * means we don't have to hope it'll catch that the functions are tail
 * recursive. *Shrug*, why not.
 *
 *   We do not use malloc ourselves:
 * This is written so the user defines what's in the nodes, allowing them
 * to allocate things however they want. It's more flexible and has no
 * downsides really except the extra templating.
 *
 *   We avoid virtual:
 * Because of the templating we can get away without it for the node class.
 * Virtual costs a jump indirect. For compare this would occur for every
 * walk down the tree searching for a node. So we don't use it.
 *
 *   We have some redundancy:
 * There's a bit of redundancy in the code with the mirror-image versions of
 * the code often both being written out explicitly. This is in hopes of better
 * optimization by the compiler. We could do it left/right as an array, thus
 * allowing moduler arithmatic to take care of the mirror cases. But, it will
 * probably not be optimized as well by the compiler.
 *
 *   Not much abstraction:
 * If you look through the cases you'll see that we avoid a LOT of coditional
 * branches by not abstracting out things like rotations and re-parenting. In
 * most cases we have some information that lets us elide some conditionals
 * Thus abstraction would improve readability but at the cost of efficiency.
 * As a core library we decided we could debug it once, and it'd be worth it.
 * 
 * Threadsafety:
 *  thread compatible
 */

// TODO(mbrewer): we should try branching on whether we are left or right subchild *first*
//    we might avoid a lot of conditionals that way.
//    note: in remove left cases lead to left cases, right to right. so no need
//      to follow right cases after left or vice-versa

#include <stdio.h>
#include <utility>
#include "panic.h"

#ifndef REDBLACK_H
#define REDBLACK_H

// Define the this in your file if you want us to do some extremely
// expensive consistancy checking - this is really useful if you're looking
// for an elusive bug
//#define REDBLACK_DEBUG
// Define this to get tons of annoying and mostly useless debugging output
// give it a try, but you probably don't want to do this.
//#define REDBLACK_DEBUG_VERBOSE

#ifdef REDBLACK_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#define PRINT_TREE() print();
#else
#define PRINT(msg)
#define PRINT_TREE()
#endif

#ifdef REDBLACK_DEBUG
#define CHECK() check()
#define CHECK_ALL() checkAll()
#else
#define CHECK()
#define CHECK_ALL()
#endif

template<typename Node_T, typename Val_T>
class RedBlackNode_base {
  public:
    // TODO(mbrewer): try making the first 2 an array
    Node_T *left;
    Node_T *right;
    Node_T *parent;
    bool red;
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


template<typename Node_T, typename Val_T>
class RedBlack{
  static_assert(std::is_same<decltype(std::declval<Node_T>().val()), Val_T>(), "Please define a method Val_T val() method on Node_T class");
  static_assert(std::is_same<decltype(Node_T::compare(std::declval<Val_T>(), std::declval<Val_T>())), int>(), "Please define a static method int compare(Val_T, Val_T) method on Node_T class");
  private:
    Node_T *root;
    void _check(Node_T *parent, Node_T *n);
    size_t _checkAll(Node_T *parent, Node_T *n);
    void _print(Node_T *n);
    void _order(void (*prefunc)(Node_T*), void (*infunc)(Node_T*), void (*postfunc)(Node_T*), void (*leaffunc)(void), Node_T *n);
  public:
    RedBlack();
    ~RedBlack();
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
    void order(void (*prefunc)(Node_T*), void (*infunc)(Node_T*), void (*postfunc)(Node_T*), void (*leaffunc)(void));
};

template<typename Node_T, typename Val_T>
RedBlack<Node_T, Val_T>::RedBlack() {
  root = nullptr;
}

template<typename Node_T, typename Val_T>
RedBlack<Node_T, Val_T>::~RedBlack() {
  if (root) {
    PANIC("Tree is being destroyed, yet still contains nodes");
  }
}

// Get functions
template<typename Node_T, typename Val_T>
Node_T *RedBlack<Node_T, Val_T>::get(Val_T v) {
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
bool RedBlack<Node_T, Val_T>::isempty(void) {
  return !root;
}

// Insert functions
template<typename Node_T, typename Val_T>
bool RedBlack<Node_T, Val_T>::insert(Node_T *new_n) {
  PRINT("Begin Insert\n");
  new_n->right = nullptr;
  new_n->left = nullptr;
  new_n->red = true;
  #ifdef REDBLACK_DEBUG_VERBOSE
  printf("inserting: ");
  new_n->print();
  printf("\n");
  #endif
  PRINT_TREE();
  CHECK_ALL();
  // We do this here, so we don't have to do the check every iteration
  if (!root) {
    PRINT("Case no root\n");
    root = new_n;
    root->parent = nullptr;
    root->red = false; // root so recolor black
    CHECK_ALL();
    return true;
  } 

  Node_T *parent = root;

  // insert it
  while (true) {
    int c = Node_T::compare(new_n->val(), parent->val());
    if (c > 0) {
      if (!parent->right) {
        parent->right = new_n;
        new_n->parent = parent;
        break;
      }
      parent = parent->right;
    } else if (c < 0) {
      if (!parent->left) {
        parent->left = new_n;
        new_n->parent = parent;
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
  PRINT_TREE();

  // balance it
  while (true) {
    // Inputs to this loop are: new_n, parent
    // Note: Case 1 (root case) is checked on insert before we get here
    // In recursive cases it's checked prior to recursion in each branch
    // We do this because we already need to check for NULL there anyway

    // Case 2, parent is black
    if (!parent->red) {
      PRINT("case 2\n");
      // If our parent is black, it's all good
      break;
    }
    // known: parent is red

    // Because parent is red, parent is not root so 
    // known: parent is red and grandparent exists

    // In addition, red nodes can only point at black nodes
    // Known: parent is red and grandparent is black

    // Case 3, uncle is also red
    // NOTE: One version of the algorithm (see rredblack.h) doesn't use case 3
    // For that version to work we recolor the parent red and it's children black
    // in cases 4 and 5, instead of parent black and children red... downside is that
    // it always propogates upwards, whereas having case 3 here is the only upwards
    // propogation, the other two get to break out of our loop.
    Node_T *grandparent = parent->parent;

    // TODO(mbrewer): use this conditional for the following case 4/5 as well?
    Node_T *uncle = grandparent->left==parent ? grandparent->right : grandparent->left;
    if (uncle && uncle->red) {
      PRINT("Case 3\n");
      // Repaint both black, so we add black everywhere.
      // Recurse on parent so we can fix that we now have more blacks than
      // the rest of the tree.
      uncle->red = false;
      parent->red = false;
      grandparent->red = true;
      // Recurse on grandparent (grandparent is red, so we have to check it) 
      new_n = grandparent;
      parent = grandparent->parent;
      // This is the only case where we haven't already checked root
      // due to other logic, so we move Case 1 into case 5 to save the
      // conditional in all other cases
      // Case 1, this is the root
      if (!parent) {
        PRINT("case 1\n");
        // root can't be red so repaint it black
        new_n->red = false;
        break;
      }
      continue;
    }
    // Known: grandparent is black, parent is red, uncle is black

    // Now we branch left/right
    if (grandparent->left == parent) {
      PRINT("Left case\n");
      // Case 4
      if (new_n == parent->right) {
        PRINT("Case 4\n");
        // make new_n's left child, parent's left
        parent->right = new_n->left;
        if (parent->right) {
          parent->right->parent = parent;
        }
        // make parent, new_n's left child
        new_n->left = parent;
        parent->parent = new_n;
        // TODO(mbrewer): This gets overwritten in Case 5
        // shortcircuiting case 5 would save 4 or so moves
        grandparent->left = new_n;
        new_n->parent = grandparent;
        // and rename for case 5
        Node_T *tmp = parent;
        parent = new_n;
        new_n = tmp;
      }
      CHECK();
      // Case 5
      PRINT("Case 5\n");
      // left - left, so we rotate right on p
      Node_T *great_grandparent = grandparent->parent;
      // make parent right, grandparent's left
      grandparent->left = parent->right;
      if (grandparent->left) {
        grandparent->left->parent = grandparent;
      }
      // make grandparent, parent's right
      parent->right = grandparent;
      grandparent->parent = parent; 
      // and recolor
      parent->red = false;
      grandparent->red = true;
      // if there's no great greatparent, we're root and we're done
      // embedded case 1
      if (!great_grandparent) {
        root = parent;
        parent->parent = nullptr;
        parent->red = false;
        // we're at root
        break;
      }
      // make p great grandparent's child
      if (great_grandparent->left == grandparent) {
        great_grandparent->left = parent;
      } else {
        great_grandparent->right = parent;
      }
      parent->parent = great_grandparent;
      break;
    } 
    PRINT("Right case\n");
    // Case 4
    if (new_n == parent->left) {
      PRINT("Case 4\n");
      // make new_n's right child, parent's right
      parent->left = new_n->right;
      if (parent->left) {
        parent->left->parent = parent;
      }
      // make parent, new_n's right child
      new_n->right = parent;
      parent->parent = new_n;
      // TODO(mbrewer): This gets overwritten in Case 5
      // shortcircuiting case 5 would save 4 moves in this case
      grandparent->right = new_n;
      new_n->parent = grandparent;
      // and rename for case 5
      Node_T *tmp = parent;
      parent = new_n;
      new_n = tmp;
    }
    CHECK();
    // Case 5
    PRINT("Case 5\n");
    // right - right, so we rotate left on p
    Node_T *great_grandparent = grandparent->parent;
    // make parent left, grandparent's right
    grandparent->right = parent->left;
    if (grandparent->right) {
      grandparent->right->parent = grandparent;
    }
    // make grandparent, parent's left
    parent->left = grandparent;
    grandparent->parent = parent; 
    // and recolor
    parent->red = false;
    grandparent->red = true;
    // if there's no great greatparent, we're root and we're done
    // embedded case 1
    if (!great_grandparent) {
      root = parent;
      parent->parent = nullptr;
      parent->red = false;
      // we're at root
      break;
    }
    // make p great grandparent's child
    if (great_grandparent->right == grandparent) {
      great_grandparent->right = parent;
    } else {
      great_grandparent->left = parent;
    }
    parent->parent = great_grandparent;
    break;
  }
  PRINT_TREE();
  CHECK_ALL();
  PRINT("insert complete\n");
  return true;
}

template<typename Node_T, typename Val_T>
Node_T *RedBlack<Node_T, Val_T>::remove(Node_T *n) {
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
    bool tmp_red;
    tmp_red = replacement->red;
    replacement->red = n->red;
    n->red = tmp_red;
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
  CHECK();

  // n has at most one non-leaf child, it's child could be red or black
  // if it's child is black, then it cannot have children. If it did
  // then then path to it's direct-leaf child would have 1 black, while
  // paths through the other would have at least 2
  // therefore... if it's child exists the child must be red
  // Thus we can elide the check for child->red in the following test

  // pre-Case -1: n is red OR n's child is red
  if (n->red || child) {
    PRINT("PreCase -1\n");
    // If child esists, just make it black, this way if we were black
    // we haven't removed any more blacks.
    // If n is red then child was already black anyway
    if (child){
      child->red = false;
    }
    // If we don't have a parent, just make the child root
    if (!n->parent) {
      root = child;
      if (child) {
        child->parent = nullptr;
      }
      CHECK();
      return n; 
    }
    // We do have a parent, so find which child we are and replace ourselves 
    Node_T *parent = n->parent;
    if (parent->left == n) {
      parent->left = child;
    } else {
      parent->right = child;
    }
    // Make sure the child points at the parent
    if(child) {
      child->parent = parent;
    }
    // in this case we have no rebalancing to do
    CHECK();
    return n;
  }
  // Known: n is black, has at most one non-leaf child, and child is black
  // child must be a leaf, otherwise the leaf hanging off it would make for more black nodes
  // than the path to our leaf child therefore:
  // Known: n is black, and has only leafs for children

  // pre-Case 0: n is root
  if (!n->parent) {
    PRINT("PreCase 0\n");
    // we already know we have no children, just unlink the node
    root = nullptr;
    // well, nothing to do 'cause there's no tree!
    CHECK();
    return n;
  }

  // We are not going to remove n yet.
  // we'll set it aside, and we'll balance with it in place, as IF it was gone
  // this greatly simplifies "parent" pointers for recursion (loop, whatever)
  Node_T *node_to_delete = n;

  // rebalancing procedure!

  // We removed a black node from all subtrees under n, and we need to fix it.
  // We can fix it by either:
  //   1) removing a black node from subtrees under n, if we can do this we stop
  //   2) adding a black node to n's sibling's tree, and recursing to n's parent
  while(true) {
    // We have to forget that n has only leaf children, it may have real children
    // in the recursive case.
    // but, n is always black, because we only recurse on black nodes
    // in any of the cases.

    // Known: n is black

    // NOTE: case 1 never triggers the first time through this loop
    // it's only used in the recursive case, so this test is wasteful
    // most of the time it's run
    // At a glance it looks like fixing this wouldn't really be worth it
    
    // Case 1:
    Node_T *parent = n->parent;
    if(!parent) {
      PRINT("Case 1 (complete)\n");
      CHECK();
      break;
    }
    
    // Known: n is black, and parent exists
    Node_T *sibling;
    if (parent->left == n) {
      sibling = parent->right;
    } else {
      sibling = parent->left;
    }

    // n is a real black node, not a leaf node. (we only recurse on black)
    // therefore n has "children", and thus has black path length at least 2 from
    // parent to n's children 
    // if sibling had no children it would have black path length only 1 so
    // Known: sibling has children (is not a leaf node)
    
    // Case 2: sibling is red
    // We want to reduce this to the case where the sibling is black
    // So we do a little rotating.
    if (sibling->red) {
      PRINT("Case 2\n");
      CHECK();
      // red nodes can't have red children so:
      // known: parent is black
      sibling->red = false;
      parent->red = true;
      Node_T *new_sibling;
      Node_T *grandparent = parent->parent;
      if (parent->right == sibling) {
        new_sibling = sibling->left;
        // make siblings left child, parent's right
        parent->right = sibling->left;
        if (new_sibling) {
          new_sibling->parent = parent;
        }
        // make parent siblings child
        sibling->left = parent;
        parent->parent = sibling;
      } else {
        new_sibling = sibling->right;
        // make siblings right child, parent's left
        parent->left = sibling->right;
        if (new_sibling) {
          new_sibling->parent = parent;
        }
        // make parent siblings child
        sibling->right = parent;
        parent->parent = sibling;
      }
      // make sibling grandparent's child
      if (grandparent) {
        if (grandparent->left == parent) {
          grandparent->left = sibling;
        } else {
          grandparent->right = sibling;
        } 
      } else {
        root = sibling;
      }
      sibling->parent = grandparent;
      // Reset for other logic
      sibling = new_sibling;
      CHECK();
      PRINT("case 2 complate\n");
      PRINT_TREE();
    }
    // Known: n is black, parent exists, sibling is black and not a leaf

    // Case3: parent is black and siblings's children are black
    if (!parent->red
        && !(sibling->left && sibling->left->red)
        && !(sibling->right && sibling->right->red)) {
      PRINT("Case 3\n");
      CHECK();
      // We're removing black from the whole subtree
      // so we recurse to fix the parent
      sibling->red = true;
      n = parent;
      CHECK();
      PRINT("case 3 complete\n");
      PRINT_TREE();
      continue;
    }
    // Known: n is black, sibling is black and not a leaf, parent is red 

    PRINT("checking case 4\n");
    // Case 4: parent is red and sibling's children are black
    if (parent->red  
        && !(sibling->left && sibling->left->red)
        && !(sibling->right && sibling->right->red)) {
      PRINT("Case 4\n");
      CHECK();
      // We get to add black back to n's subtree, so no recursion
      sibling->red = true;
      parent->red = false;
      CHECK();
      PRINT("case 4 complate\n");
      break;
    }
    // Known: n is black, parent exists, sibling is black
    //   sibling is not a leaf, one of siblings children is red

    // If n is the left child of parent
    PRINT("checking case 5\n");
    Node_T *grandparent = parent->parent;
    if (n == parent->left) {
      // Case 5: sibling's left child is red, and right-child is black
      if ((sibling->left && sibling->left->red)
          && !(sibling->right && sibling->right->red)) {
        PRINT("Case 5 on parent's left\n");
        CHECK();
        // reduce to case 6
        Node_T *s_left_child = sibling->left;
        // make slc's right child, siblings left child
        sibling->left = s_left_child->right;
        if (sibling->left)
          sibling->left->parent = sibling;
        // make sibling, slc's right child
        s_left_child->right = sibling;
        sibling->parent = s_left_child;
        // make slc parent's right child
        parent->right = s_left_child;
        s_left_child->parent = parent;
        sibling->red = true;
        s_left_child->red = false;
        // and relabel for the next case
        sibling = s_left_child;
        CHECK();
        PRINT("case 5 complate\n");
      }
      PRINT_TREE();
      // Known: n is black, parent exists, sibling is black
      //   sibling is not a leaf, siblings right child is red
      //   n is the left child of parent

      // Case 6: sibling's left child is black, and right child is red
      // Rotate Sibling and Parent, so parent is child of sibling
      PRINT("Case 6 n is on parent's left\n");
      CHECK();
      // make siblings left child, parent's right child
      parent->right = sibling->left;
      if(parent->right) {
        parent->right->parent = parent;
      }
      // make parent siblings left child
      sibling->left = parent;
      parent->parent = sibling;
      // and right child of sibling gets recolored
      // it's red, so we know it exists
      sibling->right->red = false;
    // If n is the right child of parent
    } else {
      // Case 5: sibling's right child is red, and left-child is black
      if ((sibling->right && sibling->right->red)
          && !(sibling->left && sibling->left->red)) {
        PRINT("Case 5 on parent's right\n");
        CHECK();
        // reduce to case 6
        Node_T *s_right_child = sibling->right;
        // make slc's left child, siblings right child
        sibling->right = s_right_child->left;
        if (sibling->right)
          sibling->right->parent = sibling;
        // make sibling, slc's left child
        s_right_child->left = sibling;
        sibling->parent = s_right_child;
        // make slc parent's left child
        parent->left = s_right_child;
        s_right_child->parent = parent;
        sibling->red = true;
        s_right_child->red = false;
        // and relabel for the next case
        sibling = s_right_child;
        CHECK();
        PRINT("case 5 complate\n");
      }
      // Known: n is black, parent exists, sibling is black
      //   sibling is not a leaf, siblings left child is red
      //   n is the right child of parent

      // Case 6: sibling's right child is black, and left child is red
      // Rotate Sibling and Parent, so parent is child of sibling
      PRINT("Case 6 n is on parent's right\n");
      CHECK();
      // make siblings right child, parent's left child
      parent->left = sibling->right;
      if(parent->left) {
        parent->left->parent = parent;
      }
      // make parent siblings right child
      sibling->right = parent;
      parent->parent = sibling;
      // and right child of sibling gets recolored
      // it's red, so we know it exists
      sibling->left->red = false;
    }
    // swap colors, we know sibling is black
    sibling->red = parent->red;
    parent->red = false;
    // make sibling grandparent's child
    if (grandparent) {
      if (grandparent->left == parent) {
        grandparent->left = sibling;
      } else {
        grandparent->right = sibling;
      }
    } else {
      root = sibling;
    }
    sibling->parent = grandparent;
    PRINT_TREE();
    CHECK();
    PRINT("case 6 complate\n");
    break;

  }

  PRINT("completed setup for removal\n");
  CHECK();
  // Remove node_to_delete 
  // We are not root, and node_to_delete has no true children (see above)
  Node_T *parent = node_to_delete->parent;
  if (parent->left == node_to_delete) {
    parent->left = nullptr; 
  } else {
    parent->right = nullptr;
  }
  PRINT("completed removal\n");
  CHECK_ALL();
  return node_to_delete;
} 


template<typename Node_T, typename Val_T>
void RedBlack<Node_T, Val_T>::check(void) {
  _check(nullptr, root);
  PRINT("check() passed\n");
}
 

template<typename Node_T, typename Val_T>
void RedBlack<Node_T, Val_T>::_check(Node_T *parent, Node_T *n) {
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
void RedBlack<Node_T, Val_T>::checkAll(void) {
  // root must be black
  if (root && root->red) {
    PANIC("Root node is red\n");
  }
  _checkAll(nullptr, root);
  PRINT("checkAll() passed\n");
}


template<typename Node_T, typename Val_T>
size_t RedBlack<Node_T, Val_T>::_checkAll(Node_T *parent, Node_T *n) {
  if (!n) {
    return 1;
  }
  size_t b_left = _checkAll(n, n->left);  
  size_t b_right = _checkAll(n, n->right);
  if (n->parent != parent) {
    printf("Node: ");
    n->print();
    printf(" Parent: ");
    parent->print();
    printf("\n");
    PANIC("Node is corrupt: it doesn't point to it's parent");
  }
  if (b_left != b_right) {
    PANIC("left subtree has different black count than right subree");
  }
  if (n->red) {
    // red nodes must have only black children
    if (n->parent && n->parent->red && n->red) {
      PANIC("Red node has red children\n");
    }
  } else {
    b_left += 1;  
  }
  return b_left;
}

template<typename Node_T, typename Val_T>
void RedBlack<Node_T, Val_T>::print() {
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

template<typename Node_T, typename Val_T>
void RedBlack<Node_T, Val_T>::order(void (*prefunc)(Node_T*), void (*infunc)(Node_T*), void (*postfunc)(Node_T*), void (*leaffunc)(void)) {
  _order(prefunc, infunc, postfunc, leaffunc, root);
}

template<typename Node_T, typename Val_T>
void RedBlack<Node_T, Val_T>::_order(void (*prefunc)(Node_T*), void (*infunc)(Node_T*), void (*postfunc)(Node_T*), void (*leaffunc)(void), Node_T *n) {
  if (!n) {
    leaffunc();
    return;
  }
  if (prefunc) {
    prefunc(n);
  }
  _order(prefunc, infunc, postfunc, leaffunc, n->left);
  if (infunc) {
    infunc(n);
  }
  _order(prefunc, infunc, postfunc, leaffunc, n->right);
  if (postfunc) {
    postfunc(n);
  }
}

#endif
