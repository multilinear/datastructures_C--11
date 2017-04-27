/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2014-03-22 
 *
 * Simple singly linked C++ list
 * 
 * Use:
 *  Build a "node" class yourself which inherits ListNode_base.
 *  Pass in your class as the tempalte argument to ListNode_base.
 *  e.g. class Node: public ListNode_base<Node> {
 *
 *  Create a list object, also templatized on your node class.
 *  Allocate your node objects yourself, and enqueue/dequeue as you wish
 *
 * Threadsafety:
 *  Thread compatible
 */

#include "panic.h"

#ifndef LIST_H
#define LIST_H


template<typename Node_T>
class ListNode_base {
  public:
    Node_T* next;
};

template<typename Node_T> class List{
  private:
    Node_T* _head;
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
      return Iterator(_head);
    }
    Iterator end() {
      return Iterator(nullptr);
    }

    List();
    ~List();
    bool isempty(void) const;
    void enqueue(Node_T*);
    Node_T* dequeue();
    Node_T* peek(void) const;
};

template<typename Node_T>
List<Node_T>::List(){
  _head = nullptr;
  tail = nullptr;
}

template<typename Node_T>
List<Node_T>::~List(){
  if (_head != nullptr || tail != nullptr){
    PANIC("List destroyed while not empty");
  }
}

template<typename Node_T>
void List<Node_T>::enqueue(Node_T* el){
  // We do this here rather than in a constructor as it saves setting it to
  // NULL on dequeue, which we'd have to do to ensure re-enqueuing worked.
  el->next = nullptr;
  if (tail == nullptr) {
    _head = el;
    tail = el;
    return;
  }
  tail->next = el;
  tail = el;
}

template<typename Node_T>
Node_T* List<Node_T>::dequeue(){
  auto el = _head;
  if (_head == nullptr) {
    return nullptr;
  }
  _head = _head->next;
  if (_head == nullptr) {
    tail = nullptr;
  }
  return el;
}

template<typename Node_T>
bool List<Node_T>::isempty(void) const {
  return _head == nullptr;
}

template<typename Node_T>
Node_T *List<Node_T>::peek(void) const {
  return _head; 
}

#endif
