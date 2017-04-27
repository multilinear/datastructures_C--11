/*
 * Copywrite: Matthew Brewer 2014-04-26
 *
 * When to use:
 *   Not benchmarked yet, but unlikely to beat AVL
 *
 * Simple randomized skiplist implementation.
 * We randomly select the number of levels for each node
 * so on average it comes out somewhere close to balanced.
 *
 * Time bounds:
 * Average log time for all operations
 * Worst case linear for all operations (because balance is probabalistic)
 *
 * Threadsafety:
 *   Thread compatible
 */

#include "array.h"

#ifndef SKIPLIST_H
#define SKIPLIST_H

#ifdef SKIPLIST_DEBUG
#define SKIPLIST_CHECK() check()
#else
#define SKIPLIST_CHECK()
#endif

template<typename Node_T>
class SkipListNode_base {
  public:
    Array<Node_T *> next;
		SkipListNode_base():next(){
		}
};

template<typename Node_T, typename Val_T>
class SkipList {
  private:
    Node_T *head;
    size_t length;
    size_t levels; // This value only grows
		void check(void) const;
  public:
   class Iterator {
      private:
        Node_T* n;
      public:
        Iterator(Node_T *nn) {
          n = nn;
        }
        Iterator(const Iterator& other) {
          n = other.n;
        }
        Iterator& operator=(const Iterator& other) {
          n = other.n;
					return *this;
        }
        bool operator==(const Iterator& other) const {
          return n == other.n;
        }
        bool operator!=(const Iterator& other) const {
          return n != other.n;
        }
        Iterator& operator++() {
          if (n) {
            n = n->next[0];
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
      return Iterator(head);
    }
    Iterator end() {
      return Iterator(nullptr);
    }
		SkipList();
    bool insert(Node_T *n);
    Node_T* get(Val_T v);
    void remove(Node_T *n);
    bool isempty(void) const;
		void print(void) const;
};

template<typename Node_T, typename Val_T>
SkipList<Node_T,Val_T>::SkipList() {
	length = 0;
	levels = 1;
	head = nullptr;
}

template<typename Node_T, typename Val_T>
bool SkipList<Node_T,Val_T>::insert(Node_T *newn) {
	SKIPLIST_CHECK();
  // Empty base case
  if (!head) {
    levels = 1;
    head = newn;
    newn->next.resize(1);
    newn->next[0] = nullptr;
		length++;
		SKIPLIST_CHECK();
		return true;
  }
  // Value we're inserting is already in head
  if (head->val() == newn->val()) {
    return false;
  }
  // Value is becoming the new head
  if (head->val() > newn->val()) {
    Node_T *tmp = head;
    // Replace head
    newn->next.resize(head->next.len());
    for (size_t i = 0; i<newn->next.len(); i++) {
      newn->next[i] = head->next[i];
    }
    head = newn;
    // Insert the old head, like we were going to newn
    //
    // We *could* just put it in here, but
    // The number of levels should be randomized
    // Otherwise if you put elements in in descending
    // Order all nodes will have all levels
    newn = tmp;
  }
  // Make sure head has all the levels it needs
  size_t new_levels = 0;
  for (size_t i = length; i>0; i=i/2) {
    new_levels++;
  }
  if (new_levels > levels) {
		levels = new_levels;
    head->next.resize(levels);
    head->next[levels-1] = nullptr;
  }
  // Setup our new node
  // Pick a random level... 50% chance of adding each level
  size_t rlevel = 1;
  for (size_t i = 1; i < levels; i++) {
    if (rand() % 2 == 0) {
      break;
    }
    rlevel++;
  }
  newn->next.resize(rlevel);
  // find location
  // For each level, find the link that jumps it. If it has that level put it in the middle
  Val_T v = newn->val();   
  Node_T* closest = head;
  size_t level = levels;
  while (level > 0) {
    // Find where we skip over our element
    Node_T* next = closest->next[level-1];
    if (next) {
      if (next->val() < v) {
        closest = next;
        continue;
      }
      if (next->val() == v) {
        SKIPLIST_CHECK();
        return false; 
      }
    }
    // Stick ourselves in the list
    if (newn->next.len() > level-1) {
      newn->next[level-1] = closest->next[level-1];
      closest->next[level-1] = newn;
    }
    // Start over from closest node, but at the next level down
    level--;
  }
  length++;
	SKIPLIST_CHECK();
  return true;
}

template<typename Node_T, typename Val_T>
Node_T* SkipList<Node_T,Val_T>::get(Val_T v) {
	SKIPLIST_CHECK();
  if (!head) {
    return nullptr;
  }
	if (head->val() == v) {
		return head;
	}
  Node_T* closest = head;
  size_t level = levels;
  while (level > 0) {
    Node_T* next = closest->next[level-1];
    if (next) {
      if (next->val() < v) {
        closest = next;
        continue;
      }
      if (next->val() == v) {
        return next;
      }
    }
    level--;
  }
	SKIPLIST_CHECK();
  return nullptr; 
}

template<typename Node_T, typename Val_T>
void SkipList<Node_T,Val_T>::remove(Node_T *n) {
	SKIPLIST_CHECK();
	length--;
  if (n == head) {
	  Node_T* old_head = head;
    head = n->next[0];
		if (head) {
			// Make head point to all the lists again 
			size_t level = head->next.len();
			head->next.resize(old_head->next.len());
			for (; level < head->next.len(); level++) {
				head->next[level] = old_head->next[level];
			}
		}
		SKIPLIST_CHECK();
		return;
  }
  Node_T* closest = head;
  size_t level = levels;
  while (level > 0) {
    Node_T* next = closest->next[level-1];
    if (next) {
      if (next->val() < n->val()) {
        closest = next;
        continue;
      }
      if (next == n) {
        // Take it out of the list
        closest->next[level-1] = next->next[level-1];
      }
    }
    level--;
  }
	SKIPLIST_CHECK();
  return; 
}

template<typename Node_T, typename Val_T>
bool SkipList<Node_T,Val_T>::isempty(void) const {
	SKIPLIST_CHECK();
  return length == 0;
}

template<typename Node_T, typename Val_T>
void SkipList<Node_T,Val_T>::print(void) const {
	SKIPLIST_CHECK();
	Node_T* n = head;
	printf("[");
	for (;n!=nullptr;n=n->next[0]) {
		n->print();
		if (n->next[0]) printf(",");
	}
	printf("]\n");
}

template<typename Node_T, typename Val_T>
void SkipList<Node_T,Val_T>::check(void) const {
  Node_T *n = head;
  size_t l;
  for (l = 0; l<levels-1; l++) {
    for (;n!=nullptr;n=n->next[l]) {
      if (n->next.len() > levels) {
        PANIC("skiplist broke badly");
      }
    }
  }
}
	
#endif
