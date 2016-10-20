/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2014-03-22 
 *
 * Simple doubly linked C++ list
 * 
 * Use:
 *  Build a "node" class yourself which inherits DListNode_base.
 *  Pass in your class as the tempalte argument to DListNode_base.
 *  e.g. class Node: public DListNode_base<Node> {
 *
 *  Create a list object, also templatized on your node class.
 *  Allocate your node objects yourself, and enqueue/dequeue as you wish
 *
 * Threadsafety:
 *  Thread compatible
 */

#include "panic.h"

#ifndef DLIST_H
#define DLIST_H

// Define the this in your file if you want us to do some extremely
// expensive consistancy checking - this is really useful if you're looking
// for an elusive bug
//#define LIST_DEBUG

#ifdef DLIST_DEBUG
#define CHECK() check()
#else
#define CHECK()
#endif

template<typename Node_T>
class DListNode_base {
  public: // TODO(mbrewer): make this private somehow
    Node_T* next;
    Node_T* prev;
};

template<typename Node_T> class DList {
  private:
    // data is removed at the head
    Node_T* head;
    // in this implementation data enters at the tail
    Node_T* tail;
  public:
    DList();
    ~DList();
    void enqueue(Node_T*);
    Node_T* dequeue(void);
    void remove(Node_T*);
    bool isempty();
    void check(void);
};

template<typename Node_T>
DList<Node_T>::DList() {
  head = nullptr;
  tail = nullptr;
}

template<typename Node_T>
DList<Node_T>::~DList(){
  if (head != nullptr || tail != nullptr) {
    PANIC("DList destroyed while not empty");
  }
}

template<typename Node_T>
void DList<Node_T>::enqueue(Node_T* el) {
  CHECK();
  // We do this here rather than in a constructor as it saves setting it to
  // NULL on dequeue, which we'd have to do to ensure re-enqueuing worked.
  el->next = nullptr;
  el->prev = tail;
  if (tail == nullptr) {
    head = el;
    tail = el;
    CHECK();
    return;
  }
  tail->next = el;
  tail = el;
  CHECK();
}

template<typename Node_T>
Node_T* DList<Node_T>::dequeue(void) {
  CHECK();
  auto el = head;
  if (head == nullptr) {
    return nullptr;
  }
  head = head->next;
  if (head == nullptr) {
    tail = nullptr;
  } else {
    head->prev = nullptr;
  }
  CHECK();
  return el;
}

template<typename Node_T>
void DList<Node_T>::remove(Node_T *n) {
  CHECK();
  #ifndef DLIST_DEBUG
  bool found = false;
  for(Node_T *t=head; t; t=t->next){
    if (t == n) {
      found = true;
      break;
    }
  }
  if (!found) {
    PANIC("removing node from list that it is not a part of");
  }
  #endif
  if (n == head) {
    head = n->next;
  }
  if (n == tail) {
    tail = n->prev;
  }
  if (n->prev) {
    n->prev->next = n->next;
  }
  if (n->next) {
    n->next->prev = n->prev;
  }
  CHECK();
}

template<typename Node_T>
bool DList<Node_T>::isempty() {
  return head == nullptr;
}

template<typename Node_T>
void DList<Node_T>::check(void) {
  Node_T* n;
  Node_T* last_n = nullptr;
  for (n=head; n; n = n->next) {
    if (n->prev != last_n) {
      PANIC("DList is corrupt");
    }
    last_n = n;
  }
}


#endif
