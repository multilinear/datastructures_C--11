#include <stdio.h>

#define DLIST_DEBUG
#include "dlist.h"

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
  // fill and empty at different numbers
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      L.enqueue(new Node(j));
    }
    for (j = 0; j<i; j++) {
      delete L.dequeue();
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
      delete L.dequeue();
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
      delete L.dequeue();
    }
  }
  delete L.dequeue();
  delete L.dequeue();
  printf("PASS\n");
  // And test destructor here
  return 0;
}
