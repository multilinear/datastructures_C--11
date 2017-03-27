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
#define DLIST_CHECK() check()
#else
#define DLIST_CHECK()
#endif

template<typename Node_T>
class DListNode_base {
  public:
    Node_T* next;
    Node_T* prev;
    // User must define
    // Val_T val(void) {}
    // User may override
    void print(void) {
      printf("?"); 
    }
};

template<typename Node_T, typename Val_T>
class DList {
  private:
    // data is removed at the head
    Node_T* head;
    // in this implementation data enters at the tail
    Node_T* tail;
  public:
    // In theory we *could* just ues Node_T here... but that has a major downside.
    // We'll have overridden operators for Node, so if you use that class in multiple
    // datastructure types it won't work properly (eewww). So... we keep it in a 
    // seperate class
    class Iterator {
      private:
        Node_T* n;
      public:
        Iterator() {
          n = nullptr;
        }
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
            n = n->next;
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
      DLIST_CHECK();
      return Iterator(head);
    }
    Iterator end() {
      DLIST_CHECK();
      return Iterator(nullptr);
    }

    DList();
    ~DList();
    void enqueue(Node_T*);
    void insert(Node_T*);
    Node_T* dequeue(void);
    void remove(Node_T*);
    bool isempty() const;
    Node_T *get(Val_T v);
    void print(void) const;
    void check(void) const;
    Node_T *peak();
};

template<typename Node_T, typename Val_T>
DList<Node_T, Val_T>::DList() {
  head = nullptr;
  tail = nullptr;
}

template<typename Node_T, typename Val_T>
DList<Node_T,Val_T>::~DList(){
  if (head != nullptr || tail != nullptr) {
    PANIC("DList destroyed while not empty");
  }
}

template<typename Node_T, typename Val_T>
void DList<Node_T,Val_T>::enqueue(Node_T* el) {
  DLIST_CHECK();
  // We do this here rather than in a constructor as it saves setting it to
  // NULL on dequeue, which we'd have to do to ensure re-enqueuing worked.
  el->next = nullptr;
  el->prev = tail;
  if (tail == nullptr) {
    head = el;
    tail = el;
    DLIST_CHECK();
    return;
  }
  tail->next = el;
  tail = el;
  DLIST_CHECK();
}

template<typename Node_T, typename Val_T>
void DList<Node_T,Val_T>::insert(Node_T* el) {
  this->enqueue(el);
}

template<typename Node_T, typename Val_T>
Node_T* DList<Node_T,Val_T>::dequeue(void) {
  DLIST_CHECK();
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
  DLIST_CHECK();
  return el;
}

template<typename Node_T, typename Val_T>
Node_T* DList<Node_T,Val_T>::peak(void) {
  return head;
}

template<typename Node_T, typename Val_T>
void DList<Node_T,Val_T>::remove(Node_T *n) {
  DLIST_CHECK();
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
  } else {
    n->prev->next = n->next;
  }
  if (n == tail) {
    tail = n->prev;
  } else {
    n->next->prev = n->prev;
  }
  DLIST_CHECK();
}

template<typename Node_T, typename Val_T>
bool DList<Node_T,Val_T>::isempty() const {
  return head == nullptr;
}
    
template<typename Node_T, typename Val_T>
Node_T* DList<Node_T,Val_T>::get(Val_T v){
  for (auto n = head; n != nullptr; n = n->next) {
    if (n->val() == v) {
      return n;
    }
  }
  return nullptr;
}

template<typename Node_T, typename Val_T>
void DList<Node_T,Val_T>::print(void) const {
  printf("[");
  for (auto n = head; n != nullptr; n = n->next) {
    n->print();
    if (n->next) {
      printf(",");
    }
  }
  printf("]");
}
 
template<typename Node_T, typename Val_T>
void DList<Node_T,Val_T>::check(void) const {
  Node_T* n;
  Node_T* last_n = nullptr;
  for (n=head; n; n = n->next) {
    if (n->prev != last_n) {
      PANIC("DList is corrupt");
    }
    last_n = n;
  }
  if (last_n != tail) {
    printf("list end %p != tail %p\n", last_n, tail);
    printf("head %p\n", head);
    PANIC("DList is corrupt");
  }
}


#endif
