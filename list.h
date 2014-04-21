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
  public: // TODO(mbrewer): make this private somehow
    Node_T* next;
};

template<typename Node_T> class List{
  private:
    Node_T* head;
    Node_T* tail;
  public:
    List();
    ~List();
    void enqueue(Node_T*);
    Node_T* dequeue();
};

template<typename Node_T>
List<Node_T>::List(){
  head = nullptr;
  tail = nullptr;
}

template<typename Node_T>
List<Node_T>::~List(){
  if (head != nullptr || tail != nullptr){
    PANIC("List destroyed while not empty");
  }
}

template<typename Node_T>
void List<Node_T>::enqueue(Node_T* el){
  auto tmp = head;
  // We do this here rather than in a constructor as it saves setting it to
  // NULL on dequeue, which we'd have to do to ensure re-enqueuing worked.
  el->next = nullptr;
  if (tail == nullptr) {
    head = el;
    tail = el;
    return;
  }
  tail->next = el;
  tail = el;
}

template<typename Node_T>
Node_T* List<Node_T>::dequeue(){
  auto el = head;
  if (head == nullptr) {
    return nullptr;
  }
  head = head->next;
  if (head == nullptr) {
    tail = nullptr;
  }
  return el;
}

#endif
