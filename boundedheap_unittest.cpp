#define BOUNDEDHEAP_DEBUG
#include "boundedheap.h"

class HeapNode: public BoundedHeapNode_base<HeapNode, int>{
  public:
    int value; 
    HeapNode() {};
    HeapNode(int val) { value=val; }
    int val() {
      return value;
    }
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
    void print() {
      //printf("%d", value);
      printf("%ld", (long) this);
    }
};

int main(int argc, char **argv) {
  BoundedHeap<HeapNode, int> heap;
  int i,j;
  printf("Begin BoundedHeap.h unittest\n");
  for (j=0;j<200;j++) {
    for (i=0;i<j;i++) {
      heap.push(new HeapNode(i));
    }
    for (i=0;i<j;i++) {
      delete heap.pop();
    }
    for (i=j;i>0;i--) {
      heap.push(new HeapNode(i));
    }
    for (i=j;i>0;i--) {
      delete heap.pop();
    }
  }
  for (j=0;j<200;j++) {
    for (i=0;i<j;i++) {
      heap.push(new HeapNode(rand()));
    }
    for (i=0;i<j;i++) {
      delete heap.pop();
    }
  }

  printf("PASS\n");
}
