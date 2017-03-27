/*
 * This is an implementation of a BHeap.
 * A BHeap is a datastructure invented by Myself (Matthew Brewer), which
 * combines the concepts of a B-tree and a Heap.
 *
 * When to use this:
 * Probably don't, it's slow.
 * This is the best heap I've yet written in relation to worst-case runtimes...
 * but I think a cycle-steeling-copy heap would work better.
 * heap.h is faster average time., more detail below
 *
 * Reasons to use this:
 * Gives a bound on worst-case run-time without concern for realloc implementation
 * A good realloc should avoid copying all data on an array resize for for a
 * Normal heap, but your depending on it to do that. With this impl, it doesn't matter
 * Cost relative to heap.h is ~20%
 *
 * Originally I had hoped it would have a better average case runtime than
 * a normal heap... It does not.
 * BUT... it still has a *bounded* worst-case runtime! at ~20% average case cost
 * That said... a good re-alloc makes this irrelevent.
 *
 * Concept:
 * A standard Heap implementation is flat in memory, comparisons (jumps) are
 * done between a node i and 2i+1 and 2i+2, initially these are close together
 * but in large trees they are not, so all comparisons are non-local, blowing
 * cachelines with every operation.
 *
 * A BHeap in contrast is sort of like a Heap of Heaps. The bottom level of each subheap
 * Points to more subheaps, rather than storing the data locally.
 * This makes *most* operations local (within a subheap) as we push data down through the heap
 * With occasional non-local comparisons as we cross between levels. Conceptually this
 * should make the "bubble-up" and "bubble-down" operations faster on very large datasets
 * The purpose of this code is to 1) find out if this is true 2) provide a usable
 * implementation if it is
 *
 * RESULTS: So far this appears to be ~20% slower than a normal heap, even on
 * very large datasets. Tests were run by pushing and then popping 10,000,000
 * elements. 
 * BUT it has a true runtime bounded to O(log(n)) since it never needs to copy
 * the array over to a new location.
 * 
 * Algorithm:
 * See the BoundedHeap and Heap implementations first, we re-use some of the
 * tricks from those implications. 
 * We track a "tail" subheap, and a "root" subheap, as well as a "direction" 
 * boolean (going left or going right). 
 * Each subheap is split in to 2 components the "splits" that is, actual data
 * stored just like a normal heap, and "subheaps". The subheaps array is notionally
 * appended to the end of the "splits" array, considered this way "subheaps"
 * is merely the last layer of the subheap
 *
 * One complication is that we'd like to load the splits of a level before we
 * create the next level, so we don't waste tons of ram. For this reason we load
 * the heap in a slightly awkward order
 * 1) fill the tail node "splits" 
 * 2) When the tail node is full attempt to create a new sibling node in <direction>. This means recursively looking at parent nodes until tail is not the furthest child in <direction> in that node, then selecting the next child in <direction> and recursing back down the tree to the most <!direction> children, and finally creating a new child
 * 3) If we reach the root and find tail is the most <direction> branch of the tree, flip the direction variable and create a new child under tail itself (thus starting a new level to the tree)
 * 
 * Insert: 
 * 1) Add data to the tail:
 * Base case:
 *   root subheap already exists, tail points at it, so see iteritive case
 * Iteritive case
 *   If "splits" in the tail is full then create the sibling in <direction>, set tial to this
 *   (iterate up tree looking at parents until previous node is  not the <direction> most child, Take next child in <direction> and recurse down until "subheaps" is empty. Create the sibling and append it to subheaps)
 *   If this doesn't exist, create a child of tail and set tail to this
 *   Insert the data in to tail
 * 2) Bubble up:
 * Run standard bubble-up algorithm on splits in the tail subheapS
 * If the bubble-up procedure halts, we're done
 * If we reach the root of splits look in the parent subheap and, treating "subheaps" as the bottom layer of the heap compare our root to the parent node in "splits" in the parent subheap. Again as in the normal bubble-up procedure
 * recurse our bubble-up procedure on the parent
 * 
 * Remove
 * 2) Swap the root "splits" element of the root node with the tail splits element of the tail node
 * 1) Delete the tail element of the tail node 
 * Base case:
 *   If the tail "splits" array is empty, return Null (error)
 * Iteritive case
 *   Delete The tail element of the tail node 
 *   If tail splits array is not empty, Find the sibling node in <!direction> (see procedure from insert) , then delete the tail node, and set tail to the sibling
 *   If no sibling exists set tail to parent
 * 3) Bubble down the root node
 *   Again follow the standard bubble-down procedure, but when we run out of children, just look at the roots of the subheaps in the "subheaps" array as children, and then recurse in to the subheaps.
 *
 *
 * Notes on <direction>
 * A few more notes on this concept. Within each subheap we just load in normal heap order, so 
 * everything is simple. So, think just about the linked-heap structure of subheaps, treating each
 * subheap as a node, this is an N-ary tree. It's important that when we are creating nodes we create
 * all the nodes we can at a given depth before we move on to the next depth (and remove in the same 
 * order). To accomplish this we need an ordering of the parent nodes that is stable until a level is 
 * filled (it can change whenever we start on a new level). 
 *
 * At the same time, when we finish filling a level, we want to simply add the next node as a child
 * of the previous tail, to avoid doing a complex/expensive search for the other side of the tree.
 * This implies that we fill the tree going left, and then the next level going right.
 *
 * But remember, our ordering only needs to be stable per level, for this reason we can ignore
 * The order of the leafs we are adding, and their parents, and always load them in standard heap order
 * This greatly simplifies the code... though it takes a while to get your head around.
 */

#include <stdio.h>
#include "array.h"
#include "panic.h"

#ifndef BHEAP_H
#define BHEAP_H

// Define this to see tons of detail about what the tree is doing
#ifdef BHEAP_DEBUG_VERBOSE
#define PRINT(msg) printf(msg)
#else
#define PRINT(msg)
#endif

// Define this to implement some expensive consistancy checking, this is great
// for debugging code that uses the tree as well.
// This checks all of the invariants before and after ever oparation.
#ifdef BHEAP_DEBUG
#define BHEAP_CHECK() check()
#else
#define BHEAP_CHECK()
#endif

// Forward declaration... useful for debugging output
template<typename, typename, size_t>
class BHeap;

// This is an internal datatype that users don't interact with anyway
// Protected by the "BHeap" interface.
// Thus, I'm not too worried about exposing things on the public interface... 
// You've got to be doing something rude to even GET to one of these objects.
template<typename T, typename C, size_t Size>
class SubHeap {
  public:
    // Stores the split nodes
    StaticUArray<T, Size> splits;
    // Stores the last level, which is more BHeaps
    StaticUArray<SubHeap<T, C, Size> *, Size+1> subheaps;
    SubHeap<T, C, Size> *parent;
    // Note that subheaps never move, they are only created and destroyed
    // only data moves, so this index is persistant from creation on 
    size_t parent_index;

    bool isfull() const {
      return splits.isfull() && subheaps.isfull();
    }

    bool isempty() const {
      return splits.len() == 0;
    }
    
    bool isleaf() const {
      return subheaps.isempty();
    }

    static void swap(SubHeap<T, C, Size> *subheap1, size_t index1, SubHeap<T, C, Size> *subheap2, size_t index2) {
      T tmp = subheap1->splits[index1];
      subheap1->splits[index1] = subheap2->splits[index2];
      subheap2->splits[index2] = tmp;
    }

    int compare(SubHeap<T, C, Size> *subheap1, size_t index1, SubHeap<T, C, Size> *subheap2, size_t index2) {
      return C::compare(&subheap1->splits[index1], &subheap2->splits[index2]);
    }

    void bubble_down() {
      size_t i = 0;
      size_t j;
      SubHeap<T, C, Size> * i_subheap = this;
      SubHeap<T, C, Size> * j_subheap;
      SubHeap<T, C, Size> * left_subheap;
      SubHeap<T, C, Size> * right_subheap;
      int c;
      while(true) {
        size_t left = 2*i + 1;
        size_t right = 2*i + 2;
        right_subheap = i_subheap;
        left_subheap = i_subheap;
        // If we run off splits, use subheaps
        if (left >= i_subheap->splits.len()) {
          if (i_subheap->splits.len() < Size || left >= i_subheap->subheaps.len() + Size) {
            left_subheap = NULL;
          } else {
            left_subheap = i_subheap->subheaps[left - Size];
            left = 0;
          }
        }
        if (right >= i_subheap->splits.len()) {
          if (i_subheap->splits.len() < Size || right >= i_subheap->subheaps.len() + Size) {
            right_subheap = NULL;
          } else {
            right_subheap = i_subheap->subheaps[right - Size];
            right = 0;
          }
        }
        // find the smaller value
        c = 0;
        if (!right_subheap) {
          if (!left_subheap) {
            // Turns out we have no children, we're done;
            return;
          } else {
            // We only have a right child, so use that
            c = -1;
          }
        } else {
          if (!left_subheap) {
            // We only have a left child, so use that
            c = 1;
          } else {
            // We have both, compare them! (equivelent to left - right)
            c = compare(left_subheap, left, right_subheap, right);
          }
        }
        if (c > 0) {
          j = right;
          j_subheap = right_subheap; 
        } else {
          j = left; 
          j_subheap = left_subheap; 
        }
        // if i is smaller than or equal to j, we're done
        c = compare(i_subheap, i, j_subheap, j);
        if (c <= 0) {
          return;
        } 
        swap(i_subheap, i, j_subheap, j);
        // go to where we moved the data and try again
        i = j;
        i_subheap = j_subheap;
      }
    }

    void bubble_up() {
      SubHeap<T, C, Size> *i_subheap = this;
      SubHeap<T, C, Size> *p_subheap = this;
      size_t i = i_subheap->splits.len()-1;
      size_t p;
      while(true) {
        if (i==0) {
          if (!i_subheap->parent) {
            return;
          }
          p_subheap = i_subheap->parent;
          p = (i_subheap->parent_index+Size-1)/2 ;
        } else {
          p = (i-1)/2;
        }
        int c = compare(p_subheap, p, i_subheap, i);
        // If p is smaller than or equal to i, we're done 
        if (c <= 0) {
          return;
        }
        swap(p_subheap, p, i_subheap, i);
        i = p;
        i_subheap = p_subheap;
      }
    }

    bool _check_heap_invariant(size_t i) {
      int c;
      bool res = true;

      // Check left
      size_t left = 2*i+1;
      SubHeap<T, C, Size> *left_subheap = this;
      bool found_left = true;
      if (left >= splits.len()) {
        left = left - Size;
        if (splits.len() < Size || left >= subheaps.len()) {
          found_left = false;
        } else {
          left_subheap = subheaps[left];
          left = 0;
        }
      }
      if (found_left) {
        c = compare(this, i, left_subheap, left);
        if (c > 0) {
          printf("heap is not in order\n");
          res = false;
        }
        res &= left_subheap->_check_heap_invariant(left);
      }

      // Check right
      size_t right = 2*i+2;
      SubHeap<T, C, Size> *right_subheap = this;
      bool found_right = true;
      if (right >= splits.len()) {
        right = right - Size;
        if (splits.len() < Size || right >= subheaps.len()) {
          found_right = false;
        } else {
          right_subheap = subheaps[right];
          right = 0;
        }
      }
      if (found_right) {
        c = compare(this, i, right_subheap, right);
        if (c > 0) {
          printf("heap is not in order\n");
          res = false;
        }
        res &= right_subheap->_check_heap_invariant(right);
      }
      return res;
    }

    size_t _min_depth() {
      size_t i;
      size_t res;
      size_t min;
      if (!subheaps.len() || subheaps.len() < Size) {
        return 0;
      }
      if (splits.len() != Size) {
        PANIC("Subheaps used, but splits not full\n");
      }
      min = subheaps[0]->_min_depth(); 
      for (i=1; i<subheaps.len(); i++) {
        res = subheaps[i]->_min_depth(); 
        if (res < min) {
          min = res;
        }
      }
      return min+1;
    }

    size_t _max_depth() {
      size_t i;
      size_t res;
      size_t max;
      if (!subheaps.len()) {
        return 0;
      }
      max = subheaps[0]->_max_depth(); 
      for (i=1; i<subheaps.len(); i++) {
        res = subheaps[i]->_max_depth(); 
        if (res > max) {
          max = res;
        }
      }
      return max+1;
    }

    bool _check_parent_pointers() {
      bool res = true;
      if (!subheaps.len()) {
        return res;
      }
      for (size_t i=0; i<subheaps.len(); i++) {
        if (subheaps[i]->parent != this) {
          printf("real parent=%p, node=%p, parent ptr=%p\n", this, subheaps[i], parent);
          printf("corrupt parent pointer\n"); 
          res = false;
        }
        if (subheaps[i]->parent_index != i) {
          printf("corrupt parent index\n");
          res = false;
        }
        res &= subheaps[i]->_check_parent_pointers();
      }
      return res;
    }

    SubHeap():splits(), subheaps() {
      parent = nullptr;
    }
    ~SubHeap() {
    }

    T& peak() {
      BHEAP_CHECK();
      return splits[0];
    }

    bool check() {
      bool res = true;
      res &= _check_heap_invariant(0);
      res &= _check_parent_pointers();
      if (_max_depth() - _min_depth() > 1) {
        printf("Tree depth varies by more than 1");
        res &= false;
      }
      return res;
    }

    void print() {
      //printf(" %p:", this);
      printf("(");
      if (splits.len() > 0) {
        printf("%d", splits[0]);
      }
      for (size_t i=1; i<splits.len(); i++) {
        printf(",%d", splits[i]);
      }
      if (subheaps.len() > 0) {
        subheaps[0]->print();
      }
      for (size_t i=1; i<subheaps.len(); i++) {
        printf(",");
        subheaps[i]->print();
      }
      printf(")");
    }
};



// This heap is coded as a minheap
template<typename T, typename C, size_t Size>
class BHeap {
  private:
    SubHeap<T, C, Size> root;
    SubHeap<T, C, Size> *tail;
    // Which direction should we go to find new branches of the tree
    // while *filling* the tree.
    // Swaps each time the tree gains a new "level".
    bool going_left;

    // This function not side-effect free, it actually
    // flips "going_left" back and forth. It's seperated
    // Into it's own function for convenience of control flow.
    SubHeap<T, C, Size> * find_new_tail_parent() {
      SubHeap<T, C, Size> * n = tail;
      if (!n->parent) {
        // We're root, so we're the new parent
        // We don't care if we're going left or right right now
        return n;
      } 
      if (n->parent_index < Size) {
        // Just push our new node on our parent subheap list
        return n->parent;
      } 
      // We're not root, and parent is full, search tree above parent
      // For next node to our "direction"
      n = n->parent;
      if (!going_left) {
        // Walk up the tree looking for a branch to our right
        while (true) {
          if (!n->parent) {
            going_left = true;
            return tail;
          }
          if (n->parent_index < Size) {
            break;
          }
          n = n->parent;
        }
        // Take the next branch to our right
        n = n->parent->subheaps[n->parent_index+1];
        // Descend this branch going left
        // Until a node isn't full of subheaps yet
        while (n->subheaps.len() == Size+1) {
          n = n->subheaps[0];
        }
        return n;
      } 
      // Walk up the tree looking for a branch to our right
      while (true) {
        if (!n->parent) {
          going_left = false;
          // If we're going *left* only, we're still loading the subheaps array
          // to the right, so in this case we need to start going right from the
          // leftmost of this subheap when adding a new level
          return tail->parent->subheaps[0];
        }
        if (n->parent_index > 0) {
          break;
        }
        n = n->parent;
      }
      // Take the next branch to our right
      n = n->parent->subheaps[n->parent_index-1];
      // Descend this branch going left
      // Until a node isn't full of subheaps yet
      while (n->subheaps.len() == Size+1) {
        n = n->subheaps.revi(1);
      }
      return n;
    }

    void make_new_tail() {
      PRINT("BHEAP: making tail\n");
      SubHeap<T, C, Size> * p = find_new_tail_parent();
      SubHeap<T, C, Size> * new_tail = new SubHeap<T, C, Size>();
      p->subheaps.push(new_tail);
      // A little hacky, since we just pushed it, it's the last in the list
      new_tail->parent_index = p->subheaps.len()-1;
      new_tail->parent = p;
      tail = new_tail;
      PRINT("BHEAP: done making tail\n");
    } 

    SubHeap<T, C, Size> * find_prev_tail() {
      // Base case, tail is root, nothing to delete
      if (!tail->parent) {
        return NULL;
      }      
      SubHeap<T, C, Size> * n = tail;
      // Can I just use the next in the parent?
      if (n->parent_index > 0) {
        return n->parent->subheaps[n->parent_index-1];
      }
      // We need to search more of the tree 
      n = n->parent;
      if (!going_left) {
        // Walk up the tree looking for a branch to the left
        // (because we want the previous tail)
        while (true) {
          if (!n->parent) {
            going_left = true;
            if (tail->parent->parent) {
              return tail->parent->parent->subheaps.revi(1);
            }
            return tail->parent;
          }
          if (n->parent_index > 0) {
            break;
          }
          n = n->parent;
        }
        // Take the branch just to the left
        n = n->parent->subheaps[n->parent_index-1];
        // Descend to the right, until we find a node without children
        while (n->subheaps.len() > 0) {
          n = n->subheaps.revi(1);
        }
        return n;
      }
      // Walk up the tree looking for a branch to the right
      // (because we want the previous tail)
      while (true) {
        if (!n->parent) {
          going_left = false;
          if (tail->parent->parent) {
            return tail->parent->parent->subheaps.revi(1);
          }
          return tail->parent;
        }
        if (n->parent_index < Size) {
          break;
        }
        n = n->parent;
      }
      // Take the branch just to the right 
      n = n->parent->subheaps[n->parent_index+1];
      // Descend to the left, until we find a node without children
      while (n->subheaps.len() > 0) {
        n = n->subheaps[0];
      }
      // On the last level we want the *last* element, order is flipped here (ugh)
      return n->parent->subheaps.revi(1);
    }

    void delete_tail() {
      SubHeap<T, C, Size> * n = find_prev_tail();
      if (!n) {
        printf("Can't find a new tail\n");
        return;
      }
      // Tail is always the last element in the parent, so just remove that 
      tail->parent->subheaps.drop();
      delete tail;
      tail = n;
    } 


  public:
    BHeap():root() {
      PRINT("BHEAP: Constructing\n");
      tail = &root; 
      going_left = false;
      PRINT("BHEAP: Done Constructing\n");
    }

    T& peak() {
      PRINT("BHEAP: Peak\n");
      BHEAP_CHECK();
      // TODO: This spits out uninitialized values on underflow... ick
      /*if (root.splits.empty()) {
        return NULL;
      }*/
      return root.peak();
    }

    void push(T data) {
      PRINT("BHEAP: Pushing\n");
      //printf("Pushing\n"); print();
      BHEAP_CHECK();
      // If the tail node is full, we need to find a new location
      if (tail->splits.isfull()) {
        make_new_tail();
      }
      tail->splits.push(data);
      tail->bubble_up();
      //printf("Done Pushing\n"); print();
      BHEAP_CHECK();
      return;
    }

    bool pop(T *val) {
      PRINT("BHEAP: Popping\n");
      //printf("Popping\n"); print();
      BHEAP_CHECK();
      bool found;
      if (0 == root.splits.len()) {
        // If root is empty, forget about it
        found = false;
      } else if (1 == root.splits.len()) {
        // Nothing to swap with, so we'll just pop it and return it
        found = tail->splits.pop(val);
      } else {
        // Normal case
        // Could optimize out the swap here.
        SubHeap<T, C, Size>::swap(tail, tail->splits.len()-1, &root, 0);
        found = tail->splits.pop(val);
        if (tail->isempty()){
          //printf("Deleting tail\n");
          delete_tail();
        }
        root.bubble_down();
      }
      PRINT("BHEAP: Done Popping\n");
      //printf("Done Popping\n"); print();
      BHEAP_CHECK();
      return found; 
    } 

    void check() {
      if (!root.check()) {
        print();
        PANIC("CORRUPT!");
      }
    }
    void print() {
      root.print();
      //printf("\n tail=%p", tail);
      printf("\n");
    }
};

#endif
