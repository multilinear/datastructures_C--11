#include <stdio.h>
#include "list.h"


class Node: public ListNode_base<Node> {
  public:
    Node(int val){
      value = val;
    }
    int value;
};


int main(int argc, char* argv[]) {
  List<Node> L;
  int i=0;
  // one in one out
  for (int j=0; j<1; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<1; j++) {
    printf("%i ", L.dequeue()->value);
  }
  // two in 2 out
  for (int j=0; j<2; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<2; j++) {
    printf("%i ", L.dequeue()->value);
  } 
  // two in 1 out
  for (int j=0; j<2; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  for (int j=0; j<1; j++) {
    printf("%i ", L.dequeue()->value);
  }
  // one in, add one
  for (int j=0; j<1; j++) {
    auto n = new Node(i++);
    L.enqueue(n);
  }
  // drain
  for (int j=0; j<2; j++) {
    printf("%i ", L.dequeue()->value);
  }
  printf("\n");
  printf("if numbers are in order above, pass\n");
  // And test destructor here
  return 0;
}
