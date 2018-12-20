/*
 * Copyright: Matthew Brewer (mbrewer@smalladventures.net) 2014-12-21
 *
 * When to use this:
 * Don't... use the delayed copy array (DCUArray) instead
 *
 * The purpose of this structure was to make an array with logarithmic
 * worst-case resize time that was still performant. It works, but is
 * when tested on a heap with ideal constants it was still 2.5X slower
 * than a normal doubling array implementation.
 * DCUArray destroys that with something like 30%-15% extra overhead costs
 * depending on load pattern.
 * 
 * The implentation is kept here for instructional purposes, as an interesting
 * point for comparison, and in case anyone wishes to re-run the benchmarks I
 * ran (basically, to maintain the scientific validity of my results).
 *
 * Design Decisions:
 * Resizing an array sometimes requires copying the memory
 * this can be a problem if you care about the wort-case
 * runtime of an algorithm.
 * It does work to that extent. Strictly speaking it makes every
 * operation logarithmic in return for making the worst case
 * logarithmic instead of linear. 
 * That said, the constant factors are likely too poor for
 * this to be generally all that useful, outside of say
 * realtime applications. I'm still persuing better options.
 *
 * Threadsafety:
 *   thread compatible
 */

#include "panic.h"
#include "array.h"

#ifndef TREEARRAY_H
#define TREEARRAY_H

// Define "TREEARRAY_DEBUG" to enable bounds-checking
// If enabled this makes these arrays far more expensive than C-style
#ifdef TREEARRAY_DEBUG
#define TREEARRAY_CHECK(i) check(i)
#else
#define TREEARRAY_CHECK(i)
#endif

template<size_t SizeBits>
class TreeArrayNode;

template<size_t SizeBits>
class TreeArrayLimb;


// Having them share this ensures that we can get "parent" without knowing which
// type we're looking at... this eases coding considerably
template<size_t SizeBits>
class TreeArrayNode {
  public:
    TreeArrayLimb<SizeBits> *parent;

  TreeArrayNode() {
    parent=nullptr;
  }
};

// Note: SizeBits makes for fast computation, but you can't set the block
// Size in a sane way... If you want 100 items to fit, you can't do the math in
// a machine independant manner (that I know of). We need to find a way to
// either to "log_2()" in the preprocessor, or well... actually, I think we need that.
template<size_t SizeBits>
class TreeArrayLimb: public TreeArrayNode<SizeBits> {
  public:
    // We use a used array here because it greatly simplifies
    // the logic for adding/removing nodes... yeah we could compute
    // it, but it gets ugly (and thus slow) fast.
    //
    // This Array is sized so this entire object will come out
    // as 1<<SizeBits... We hope. It's not guaranteed due to packing being
    // uncertain, but it should usually work.
    /*StaticUArray<
      TreeArrayNode<SizeBits>,
      ((1<<SizeBits) - sizeof(StaticUArray<int,0>))/sizeof(TreeArrayNode<SizeBits>*)
      > ar;*/
    //StaticUArray<TreeArrayNode<SizeBits>*, (1<<SizeBits)/sizeof(TreeArrayNode<SizeBits>*)> ar;
    StaticUArray<TreeArrayNode<SizeBits>*, 1<<SizeBits> ar;

    bool push(TreeArrayNode<SizeBits>* n) {
      if(!ar.push(n)) {
        return false;
      }
      n->parent = this;
      return true;
    }

    bool pop(TreeArrayNode<SizeBits>** n) {
      return ar.pop(n);
    }
};

template<typename T, size_t SizeBits>
class TreeArrayLeaf: public TreeArrayNode<SizeBits> {
  public:
    // Notice, this is not a used array... we don't need it here
   //StaticArray<T,(1<<SizeBits)/sizeof(T)> ar;
   StaticArray<T,1<<SizeBits> ar;
};

// SizeBits is the power of 2 we should use for a block size. Note that the
// allocated size is this times sizeof(T)
// e.g. 1 means 2, 8 means 256. 
// We use powers of 2 because the math is much faster (exponents don't need
// loops)
// TODO(mbrewer): A lot of these "size_t"s can probably be "uint16_t"s
// We could limit it to 16 bits, since blocks over ~4K or so make little sense anyway
template<typename T, size_t SizeBits>
class TreeArray {
  private:
    // Invariants:
    // - Limb nodes contain no data
    // - Limb nodes always have at least 1 child.
    // - Leaf nodes only appear at depth 1 (the bottom level), and have no children
    // - The tree is always "complete" all branches have the same depth
    // - Depth is always the depth of the tree (if root=tail depth=1)
    // - Root is a Limb node with at least 2 children, or is the only node (thus a leaf node)
    // - Enough leaf nodes are exist to accomidate "length" elements of data
    TreeArrayNode<SizeBits> *root;
    TreeArrayLeaf<T,SizeBits> *tail;
    size_t depth;
    size_t length;

    void push_to_parent(TreeArrayNode<SizeBits>* sibling, TreeArrayNode<SizeBits>* n) {
      // If the tree is empty, make the new node the tree
      if (sibling == nullptr) {
        root = n;
        tail = (TreeArrayLeaf<T,SizeBits>*) n;
        depth = 1;
        return;
      }
      // If we hit the top of the tree, add a layer
      if (sibling->parent == nullptr) {
        depth++;
        auto new_root = new TreeArrayLimb<SizeBits>();
        new_root->push(root);
        new_root->push(n);
        root = (TreeArrayNode<SizeBits>*) new_root;
        return;
      }
      // If the suggested parent has space, use it
      if (sibling->parent->push(n)) {
        return;
      }
      // If the sibling parent is full create a new node
      // and get it a parent (starting with the parent of the sibling's parent)
      auto nn = new TreeArrayLimb<SizeBits>();
      // We push first so it's tail recursive
      nn->push(n);
      push_to_parent(sibling->parent, nn);
    }

    TreeArrayNode<SizeBits>* pop_from_parent(TreeArrayLimb<SizeBits>* parent) {
      if (parent == nullptr) {
        // Occurs if we delete the last node (freeing the structure)
        root = nullptr;
        tail = nullptr;
        depth = 0;
        return nullptr;
      }
      TreeArrayNode<SizeBits> *res; // This is what our caller already freed
      if(!parent->ar.pop(&res)) {
        PANIC("Attempt to delete node from parent failed");
      }
      if(parent->ar.size() == 0)  {
        // Check if the node is now empty
        auto parent_parent = parent->parent;
        delete parent;
        parent = (TreeArrayLimb<SizeBits>*) pop_from_parent(parent_parent);
        if (!parent) {
          // This should never happen... 
          // If the root was the only node, parent==nullptr
          // If the root had more than one child, ar.size() > 0
          // Root can't have only one child
          PANIC("pop_from_parent(parent_parent) returned null");
        }
      } else if(parent == root && parent->ar.size() == 1) {
        // If root has a singleton child, just make that child root
        root = parent->ar[0];
        root->parent = nullptr;
        delete parent;
        depth--;
        return root;
      }
      // Return the tail (or transitive parent thereof)
      return parent->ar[parent->ar.size()-1];
    }

    T& get(size_t i) {
      // If depth is 0 we crash... good
      // If depth is 1 we index directly into root
      TreeArrayNode<SizeBits>* n = root;
      for (size_t l = depth-1; l>0; l--) {
        size_t index = i>>(l*SizeBits);
        i = i - (index<<(l*SizeBits));
        n = ((TreeArrayLimb<SizeBits>*)n)->ar[index]; 
      }
      return ((TreeArrayLeaf<T,SizeBits>*)n)->ar[i];
    }

  public:
    TreeArray() {
      root = nullptr; 
      tail = nullptr;
      depth = 0;
      length = 0;
    }
    TreeArray(size_t new_length) {
      root = nullptr;
      tail = nullptr;
      depth = 0;
      length = 0;
      resize(new_length);
    }
    TreeArray(T init[], size_t new_length) {
      root = nullptr;
      tail = nullptr;
      depth = 0;
      length = 0;
      resize(new_length);
      for(size_t i = 0; i < new_length; i++) {
        get(i) = init[i];
      }
    }
    ~TreeArray() {
      resize(0);
    }
    void resize(size_t new_length) {
      // Number of nodes needed
      size_t existing_nodes = (length + (1<<SizeBits) - 1) >> SizeBits;
      size_t new_nodes = (new_length + (1<<SizeBits) - 1) >> SizeBits;
      if (new_nodes == existing_nodes) { 
        length = new_length;
        return;
      }
      if (new_nodes > existing_nodes) {
        // Alloc
        for (size_t i=existing_nodes; i<new_nodes; i++) {
          auto ln = new TreeArrayLeaf<T,SizeBits>();
          push_to_parent(tail, ln);
          tail = ln;
        }
        length = new_length;
        return;
      }
      // Dealloc
      for (size_t i=new_nodes; i<existing_nodes; i++) {
        auto parent = tail->parent;
        delete tail;
        tail = (TreeArrayLeaf<T,SizeBits>*) pop_from_parent(parent);
      }
      length = new_length;
    }
    size_t size() const {
      return length;
    }
    bool isempty() const {
      return !length;
    }
    bool isfull() const {
      return false;
    }
    operator bool() const {
      return length;
    }
    void swap(size_t i, size_t j) {
      TREEARRAY_CHECK(i);
      TREEARRAY_CHECK(j);
      T tmp = get(i);
      get(i) = get(j);
      get(j) = tmp;
    }
    // ** Common functions
    T& operator[](size_t index) {
      TREEARRAY_CHECK(index);
      return get(index);
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      TREEARRAY_CHECK(length-index);
      return get(length-index);
    }
    // ** Check
    void check(size_t index) {
      if (index >= length) {
        printf("\nPANIC: index=%lu length=%lu\n", index, length);
        PANIC("Array access out of bounds\n");
      }
    }
};

/*// SizeBits is the power of 2 we should use for a block size. Note that the
// allocated size is this times sizeof(T)
// e.g. 1 means 2, 8 means 256. 
// We use powers of 2 because the math is much faster (exponents don't need
// loops)
// TODO(mbrewer): A lot of these "size_t"s can probably be "uint16_t"s
// We could limit it to 16 bits, since blocks over ~4K or so make little sense anyway
template<typename T, size_t SizeBits>
class TreeUArray {
  private:
    // Invariants:
    // - Limb nodes contain no data
    // - Limb nodes always have at least 1 child.
    // - Leaf nodes only appear at depth 1 (the bottom level), and have no children
    // - The tree is always "complete" all branches have the same depth
    // - Depth is always the depth of the tree (if root=tail depth=1)
    // - Root is a Limb node with at least 2 children, or is the only node (thus a leaf node)
    // - Enough leaf nodes are exist to accomidate "length" elements of data
    TreeArrayNode<SizeBits> *root;
    TreeArrayLeaf<T,SizeBits> *tail;
    size_t depth;
    size_t length;

    void push_to_parent(TreeArrayNode<SizeBits>* sibling, TreeArrayNode<SizeBits>* n) {
      // If the tree is empty, make the new node the tree
      if (sibling == nullptr) {
        root = n;
        tail = (TreeArrayLeaf<T,SizeBits>*) n;
        depth = 1;
        return;
      }
      // If we hit the top of the tree, add a layer
      if (sibling->parent == nullptr) {
        depth++;
        auto new_root = new TreeArrayLimb<SizeBits>();
        new_root->push(root);
        new_root->push(n);
        root = (TreeArrayNode<SizeBits>*) new_root;
        return;
      }
      // If the suggested parent has space, use it
      if (sibling->parent->push(n)) {
        return;
      }
      // If the sibling parent is full create a new node
      // and get it a parent (starting with the parent of the sibling's parent)
      auto nn = new TreeArrayLimb<SizeBits>();
      // We push first so it's tail recursive
      nn->push(n);
      push_to_parent(sibling->parent, nn);
    }

    TreeArrayNode<SizeBits>* pop_from_parent(TreeArrayLimb<SizeBits>* parent) {
      if (parent == nullptr) {
        // Occurs if we delete the last node (freeing the structure)
        root = nullptr;
        tail = nullptr;
        depth = 0;
        return nullptr;
      }
      TreeArrayNode<SizeBits> *res; // This is what our caller already freed
      if(!parent->ar.pop(&res)) {
        PANIC("Attempt to delete node from parent failed");
      }
      if(parent->ar.size() == 0)  {
        // Check if the node is now empty
        auto parent_parent = parent->parent;
        delete parent;
        parent = (TreeArrayLimb<SizeBits>*) pop_from_parent(parent_parent);
        if (!parent) {
          // This should never happen... 
          // If the root was the only node, parent==nullptr
          // If the root had more than one child, ar.size() > 0
          // Root can't have only one child
          PANIC("pop_from_parent(parent_parent) returned null");
        }
      } else if(parent == root && parent->ar.size() == 1) {
        // If root has a singleton child, just make that child root
        root = parent->ar[0];
        root->parent = nullptr;
        delete parent;
        depth--;
        return root;
      }
      // Return the tail (or transitive parent thereof)
      return parent->ar[parent->ar.size()-1];
    }

    T& get(size_t i) {
      // If depth is 0 we crash... good
      // If depth is 1 we index directly into root
      TreeArrayNode<SizeBits>* n = root;
      for (size_t l = depth-1; l>0; l--) {
        size_t index = i>>(l*SizeBits);
        i = i - (index<<(l*SizeBits));
        n = ((TreeArrayLimb<SizeBits>*)n)->ar[index]; 
      }
      return ((TreeArrayLeaf<T,SizeBits>*)n)->ar[i];
    }

  public:
    TreeUArray() {
      root = nullptr; 
      tail = nullptr;
      depth = 0;
      length = 0;
    }
    TreeUArray(size_t new_length) {
      root = nullptr;
      tail = nullptr;
      depth = 0;
      length = 0;
      resize(new_length);
    }
    TreeUArray(T init[], size_t new_length) {
      root = nullptr;
      tail = nullptr;
      depth = 0;
      length = 0;
      resize(new_length);
      for(size_t i = 0; i < new_length; i++) {
        get(i) = init[i];
      }
    }
    ~TreeUArray() {
      resize(0);
    }
    void resize(size_t new_length) {
      // Number of nodes needed
      size_t existing_nodes = (length + (1<<SizeBits) - 1) >> SizeBits;
      size_t new_nodes = (new_length + (1<<SizeBits) - 1) >> SizeBits;
      if (new_nodes == existing_nodes) { 
        length = new_length;
        return;
      }
      if (new_nodes > existing_nodes) {
        // Alloc
        for (size_t i=existing_nodes; i<new_nodes; i++) {
          auto ln = new TreeArrayLeaf<T,SizeBits>();
          push_to_parent(tail, ln);
          tail = ln;
        }
        length = new_length;
        return;
      }
      // Dealloc
      for (size_t i=new_nodes; i<existing_nodes; i++) {
        auto parent = tail->parent;
        delete tail;
        tail = (TreeArrayLeaf<T,SizeBits>*) pop_from_parent(parent);
      }
      length = new_length;
    }
    size_t size() {
      return length;
    }
    void swap(size_t i, size_t j) {
      TREEARRAY_CHECK(i);
      TREEARRAY_CHECK(j);
      // TODO(mbrewer): We can probably do better than this
      T tmp = get(i);
      get(i) = get(j);
      get(j) = tmp;
    }
    void push(T data) {
      resize(length+1);
      get(length-1) = data;
    }
    bool pop(T *val) {
      if (length > 0) {
        *val = get(length-1);
        resize(length-1);
        return true;
      }
      return false;
    }
    void drop() {
      if (length > 0) {
        resize(length-1);
      }
    }
    // ** Common functions
    T& operator[](size_t index) {
      TREEARRAY_CHECK(index);
      return get(index);
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      TREEARRAY_CHECK(length-index);
      return get(length-index);
    }
    // ** Check
    void check(size_t index) {
      if (index >= length) {
        printf("\nPANIC: index=%lu length=%lu\n", index, length);
        PANIC("Array access out of bounds\n");
      }
    }
};*/

// This is an array that's designed to change in size a lot
// This is for use in queues and stacks and that sort of thing
// It's a doubling array, including memory reclamation on downsizing
// "size()" returns the actively used portion of the array, not the
// Total available size
template<typename T, size_t SizeBits> 
class TreeUArray {
  private:
    TreeArray<T, SizeBits> ar;
  public:
    TreeUArray() { }
    TreeUArray(size_t size): ar(size) { }
    TreeUArray(T input[], size_t input_l): ar(input, input_l) { }
    TreeUArray(TreeUArray<T,SizeBits>* input): ar(input->size()) {
      array_copy<TreeUArray<T,SizeBits>, TreeUArray<T,SizeBits>>(this, input);
    }
    void push(T data) {
      size_t len = ar.size();
      ar.resize(len+1);
      ar[len] = data;
    }
    bool pop(T *val) {
      size_t len = ar.size();
      if (len > 0) {
        *val = ar[len-1];
        ar.resize(len-1);
        return true;
      }
      return false;
    }
    void resize(size_t size) {
      ar.resize(size);
    }
    void drop() {
      size_t len = ar.size();
      if (len > 0) {
        ar.resize(len-1);
      }
    }
    T& operator[](size_t index) {
      ARRAY_CHECK(index);
      return ar[index];
    }
    // Works like negative indices in python (1 is last element)
    T& revi(size_t index) {
      ARRAY_CHECK(ar.size()-index);
      return ar[ar.size()-index];
    }
    size_t size() const {
      return ar.size();
    }
    bool isfull() const {
      return false;
    }
    bool isempty() const {
      return ar.isempty();
    }
    operator bool() const {
      return ar;
    }
    size_t capacity() const {
      return ar.size();
    }
    void swap(size_t i, size_t j) {
      TREEARRAY_CHECK(i);
      TREEARRAY_CHECK(j);
      T tmp = ar[i];
      ar[i] = ar[j];
      ar[j] = tmp;
    }
    void check(size_t index) const {
      size_t length = ar.size();
      if (index >= length) {
        PANIC("Array access out of bounds\n");
      }
    }
};
#endif
