#include <stdio.h>

#define DLIST_DEBUG
#include "dlist.h"
#include "panic.h"

class Node: public DListNode_base<Node> {
  public:
    Node(int val){
      value = val;
    }
    int value;
};

int main(int argc, char* argv[]) {
  printf("Begin DList.h test\n");
  DList<Node> L;
  int i;
  int j;
  Node *n;
  // fill and empty at different numbers
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      L.enqueue(new Node(j));
    }
    for (j = 0; j<i; j++) {
      n = L.dequeue();
      if (!n) {
        PANIC("no node dequeued");
      }
      delete n;
    }
  }
  // add an element
  L.enqueue(new Node(-1));
  // now put elements in and pull them out, but leaving one
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      L.enqueue(new Node(j));
    }
    for (j = 0; j<i; j++) {
      n = L.dequeue();
      if (!n) {
        PANIC("no node dequeued");
      }
      delete n;
    }
  }
  // add another element
  L.enqueue(new Node(-1));
  // now put elements in and pull them out, but leaving two
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      L.enqueue(new Node(j));
    }
    for (j = 0; j<i; j++) {
      n = L.dequeue();
      if (!n) {
        PANIC("no node dequeued");
      }
      delete n;
    }
  }
  n = L.dequeue();
  if (!n) {
    PANIC("no node dequeued");
  }
  delete n;
  n = L.dequeue();
  if (!n) {
    PANIC("no node dequeued");
  }
  delete n;

  // Test underflow
  if (L.dequeue()) {
    PANIC("Dequeued non-existant data");
  }
  if (!L.isempty()) {
    PANIC("Dlist reports not empty while empty");
  }
    
  // Test remove
  i=0;
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      auto tmp = new Node(i);
      L.enqueue(tmp);
      if (i==j) {
        n = tmp;
      }
    }
    L.remove(n);
    delete n;
    n = L.dequeue();
    if (L.isempty()) {
      PANIC("Dlist reports empty while not empty");
    }
    if (!n) {
      PANIC("no node dequeued");
    }
    delete n;
    n = L.dequeue();
    if (!n) {
      PANIC("no node dequeued");
    }
    delete n;
    if (L.dequeue()) {
      PANIC("underflow should return false");
    }
  }

  // test iterator
  int x;
  for (x = 0; x < 10; x++) {
    auto n = new Node(x);
    L.enqueue(n);
  }
  auto it = L.begin();
  for (x = 0; x < 10; x++) {
    if (it->value != x) {
      PANIC("Iterator is broken");
    }
    ++it;
  }
  if (it != L.end()) {
    PANIC("Iterator isn't at the end");
  }
  for (x=0; x<10; x++) {
    delete L.dequeue();
  }

  printf("PASS\n");
  // And test destructor here
  return 0;
}
