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
    check(L.dequeue()->value);
  }
  // two in 2 out
  for (int j=0; j<2; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<2; j++) {
    check(L.dequeue()->value);
  } 
  // two in 1 out
  for (int j=0; j<2; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<1; j++) {
    check(L.dequeue()->value);
  }
  // one in, add one
  for (int j=0; j<1; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  // drain
  for (int j=0; j<2; j++) {
    check(L.dequeue()->value);
  }
  printf("PASS\n");
  // And test destructor here
  return 0;
}
