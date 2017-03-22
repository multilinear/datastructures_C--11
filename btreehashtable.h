/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A hashtable implementation, much like open chaining, but using a btree in place
 * of the standard linked-list.
 *
 * resizes up when size is < 2x data it contains
 * resizes down when size is > 4x data it contains
 *
 * Worst case operation is linear per op due to linear rehash
 * Next worst case is all elements hash collide, and operations are log(N)
 * 
 */ 

#include "panic.h"
#include "array.h"
#include "btree.h"

#ifndef BTREE_HASHTABLE_H
#define BTREE_HASHTABLE_H

#define MINSIZE 16
#define ARITY 64

// We need a wrapper node so we can track "hs" (hash size) that way we
// can track what we've rehashed when we resize
template <typename Data_T>
class BTreeHashTableNode {
	public:
		size_t hs;
		Data_T data;
		BTreeHashTableNode() {}
		BTreeHashTableNode(const Data_T& set_data, size_t set_hs) {
			hs = set_hs;
			data = set_data;
		}
};

// We have to proxy comprison through our wrapper node
template <typename Data_T, typename Val_T, typename HC>
class BTreeHashTableComp {
  public:
    // function to extract val from node
    // return type must be comparable
    // value must be unique for nodes that should be unique
    static const Val_T val(const BTreeHashTableNode<Data_T> &n) {
      return HC::val(n.data);
    }
    // function to compare to values
    static const int compare(const Val_T val1, const Val_T val2) {
			return HC::compare(val1, val2);
    }
    // function to hash val
    // this should be well distributed, but uniqueness is not necessary
    static size_t hash(Val_T v) {
      return HC::hash(v);
    }
    // Just supplying this since BTree wants it
    // TODO: Fix this interface, somehow make it optional?
    static void printT(const BTreeHashTableNode<Data_T> &n) {
      HC::printT(n.data);
    }
};

template <typename Data_T, typename Val_T, typename HC>
class BTreeHashTable {
  private:
    Array<BTree<BTreeHashTableNode<Data_T>, Val_T, BTreeHashTableComp<Data_T,Val_T, HC>, ARITY>> table;
    size_t count = 0;
    void check_sizeup(void);
    void check_sizedown(void);
  public:
    BTreeHashTable();
    BTreeHashTable(size_t s);
    ~BTreeHashTable();
    bool insert(const Data_T& data);
    Data_T* get(Val_T key);
    void remove(Val_T key, Data_T* data);
    bool isempty(void) const; 
    void resize(size_t s);
    void print(void);
};

template <typename Data_T, typename Val_T, typename HC>
BTreeHashTable<Data_T, Val_T, HC>::BTreeHashTable():table(MINSIZE) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<MINSIZE; ++i) {
    table[i] = BTree<BTreeHashTableNode<Data_T>, Val_T, BTreeHashTableComp<Data_T,Val_T, HC>, ARITY>();
  }
}

template <typename Data_T, typename Val_T, typename HC>
BTreeHashTable<Data_T, Val_T, HC>::BTreeHashTable(size_t s):table(s) {
  // We have to initialize the lists since
  // array doesn't construct objects it contains
  for (size_t i=0; i<s; ++i) {
    table[i] = BTree<BTreeHashTableNode<Data_T>, Val_T, BTreeHashTableComp<Data_T,Val_T, HC>, ARITY>();
  }
}

template <typename Data_T, typename Val_T, typename HC>
BTreeHashTable<Data_T, Val_T, HC>::~BTreeHashTable() {
  for (size_t i=0; i<table.len(); ++i) {
    // explicitly destruct since array doesn't do that
		// Note that btree frees anything it contains (though it does not destroy it)
    table[i].~BTree();
  }
};

template <typename Data_T, typename Val_T, typename HC>
bool BTreeHashTable<Data_T, Val_T, HC>::insert(const Data_T& data) {
  check_sizeup();
  Val_T v = HC::val(data);
  size_t i = HC::hash(v) % table.len();
  if (table[i].insert(BTreeHashTableNode<Data_T>(data, table.len()))) {
    count++;
    return true;
  }
  return false;
}

template <typename Data_T, typename Val_T, typename HC>
Data_T* BTreeHashTable<Data_T, Val_T, HC>::get(Val_T key) {
  size_t i = BTreeHashTableComp<Data_T,Val_T, HC>::hash(key) % table.len();
  auto n = table[i].get(key);
  if (!n) {
    return nullptr;
  }
  return &(n->data);
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::remove(Val_T v, Data_T *data) {
  // Note, if n is not in the hashtable, this will cause
  // some nasty corruption.
  size_t i = BTreeHashTableComp<Data_T,Val_T, HC>::hash(v) % table.len();
  BTreeHashTableNode<Data_T> tmp;
  table[i].remove(v, &tmp); 
  *data = tmp.data;
  count--;
  check_sizedown();
}

template <typename Data_T, typename Val_T, typename HC>
bool BTreeHashTable<Data_T, Val_T, HC>::isempty(void) const {
  return count == 0;
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::resize(size_t s) {
  // nothing to do
  if (s == table.len()) {
    return;
  }
  // If increasing size, we resize before we rehash
  size_t old_size = table.len();
  if (s > table.len()) {
    table.resize(s);
    // We have to initialize the btrees since
    // array doesn't construct objects it contains
    for (size_t i=old_size; i<s; ++i) {
      table[i] = BTree<BTreeHashTableNode<Data_T>, Val_T, BTreeHashTableComp<Data_T,Val_T, HC>, ARITY>(); 
    }
  }
  // TODO: As written this procedure is worst case N^2log(N), when it should be N... ouch!
  // If we make resize a slow migration to a new array though, hs can go away and this problem
  // is solved
  // Rehash everything based on "s"
  size_t i = 0;
  while (i<old_size) {
    // Find a node that needs to be rehashed
    auto n = table[i].begin();
    for (; n != table[i].end() && n->hs == s; n++) {}
    if (n == table[i].end()) {
      // Nothing left in this tree, go to the the next
      i++;
      continue;
    }
    // remove it
    Val_T v = BTreeHashTableComp<Data_T,Val_T,HC>::val(*n);
    BTreeHashTableNode<Data_T> data;
    if (!table[i].remove(v, &data)) {
      PANIC("Value wasn't in Tree!\n"); 
    }
    // rehash it
    size_t new_index = BTreeHashTableComp<Data_T,Val_T, HC>::hash(v) % s;
    data.hs = s; // set hashsize to the new size
    table[new_index].insert(data);
  }
  // If decreasing we resize after
  if (s < old_size) {
    // Destroy trees in rest of array no longer in use
    for (size_t i=s; i<old_size; ++i) {
      table[i].~BTree();
    }
    table.resize(s);
  }
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::check_sizedown(void) {
  // If it's under a quarter full resize down
  if (table.len() > 4*count) {
    size_t s = table.len() / 2;
    if (s < MINSIZE) {
      s = MINSIZE;
    }
    resize(s); 
  }
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::check_sizeup(void) {
  // If it's over half-full resize up
  if (table.len() < 2*count) {
    resize(table.len()*2); 
  } 
}

template <typename Data_T, typename Val_T, typename HC>
void BTreeHashTable<Data_T, Val_T, HC>::print(void) {
	printf("[\n");
  for (size_t i=0; i<table.len(); ++i) {
		printf("  ");
    table[i].print();
  }
	printf("]\n");
}
#endif
