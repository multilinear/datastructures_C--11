#include <stdio.h>
#include "list.h"

int last=-1;

class Node: public ListNode_base<Node> {
  public:
    Node(int val){
      value = val;
    }
    int value;
};

void check(int n) {
  if (last+1 != n) {
    PANIC("Queue is not acting like a queue"); 
  }
  last = n;
} 

int main(int argc, char* argv[]) {
  printf("Begin List.h test\n");
  List<Node> L;
  int i=0;
  // one in one out
  for (int j=0; j<1; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<1; j++) {
    auto n = L.dequeue();
    check(n->value);
    delete n;
  }
  // two in 2 out
  for (int j=0; j<2; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<2; j++) {
    int pv = L.peek()->value;
    auto n = L.dequeue();
    if (pv != n->value) {
      PANIC("peak doesn't match next dequeue");
    }
    check(n->value);
  } 
  // two in 1 out
  for (int j=0; j<2; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<1; j++) {
    auto n = L.dequeue();
    check(n->value);
    delete n;
  }
  if (L.isempty()) {
    PANIC("not empty queue reports empty");
  }
  // one in, add one
  for (int j=0; j<1; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  // drain
  for (int j=0; j<2; j++) {
    auto n = L.dequeue();
    check(n->value);
    delete n;
  }
  // Check underflow
  if (L.dequeue()) {
    PANIC("Empty queue returned a node");
  }
  if (L.peek()) {
    PANIC("Empty queue let me peak at a node");
  }
  if (!L.isempty()) {
    PANIC("empty queue reports not empty");
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
