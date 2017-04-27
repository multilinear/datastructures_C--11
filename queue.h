/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2016-12-18 
 *
 * This is a wrapper for the simple list implementation in list.h.
 * This converts list.h's more flexible but clunky external allocation
 * interface to an easy to use, but less efficient and flexible internal
 * allocation... Making fast development easier.
 *
 * Design decisions:
 *   there are several ways to implement this, but for lazy usage (the target of this wrapper)
 *   a simple malloc-based list is simple, robust, and fairly performant. If you need
 *   stricter properties you can use list.h dlist.h or ringbuffer.h
 *
 * Threadsafety:
 *   Thread compatible
 */


#include "list.h"

#ifndef QUEUE_H
#define QUEUE_H
template<typename T>
class QueueNode: public ListNode_base<QueueNode<T>> {
  public:
    T data;
};

template<typename T>
class Queue {
  private:
    List<QueueNode<T>> l;
  public:

		// This looks identical to "list.h" iterator, but it's not
		// This dereferences to the DATA, instead of to the node.
    class Iterator {
      private:
        QueueNode<T>* n;
      public:
        Iterator(QueueNode<T>* node) {
          n = node;
        }
        Iterator(const Iterator& other) {
          n = other.n;
        }
        Iterator& operator=(const Iterator& other) {
          n = other->_head;
        }
        bool operator==(const Iterator& other) const {
          return n == other.n;
        }
        bool operator!=(const Iterator& other) const {
          return n != other.n;
        }
        Iterator& operator++() {
          if (n) {
						// TODO(mbrewer): This is a bit more invasive than ideal
            n = n->next;
          }
          return *this;
        } 
        Iterator operator++(int) {
          Iterator tmp(*this);
          ++(*this);
          return tmp;
        } 
        T& operator*() {
          return n->data;
        }
        T* operator->() {
          return &(n->data);
        }
    };
    Iterator begin() {
      return Iterator(l.peek());
    }
    Iterator end() {
      return Iterator(nullptr);
    }
    Queue():l() {
    }
    ~Queue() {
      for (auto n = l.dequeue(); n; n=l.dequeue()) {
        delete n;
      }
    }
    void enqueue(T& data) {
      auto n = new QueueNode<T>();
      n->data = data;
      l.enqueue(n);
    }
    bool dequeue(T* data) {
      auto n = l.dequeue();
      if (!n) {
        return false;
      }
      *data = n->data;
      delete n;
      return true;
    }
    bool isempty() const {
      return l.peek()==nullptr;
    }
    operator bool() const {
      return l.peek();
    }
};

#endif
