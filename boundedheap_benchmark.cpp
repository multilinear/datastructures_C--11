//#define BOUNDEDHEAP_DEBUG
#include "boundedheap.h"

#define TEST_SIZE 100000000

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

HeapNode heap_nodes[TEST_SIZE];

int main(int argc, char **argv) {
  BoundedHeap<HeapNode, int> heap;
  int i,j;
  printf("Begin BoundedHeap.h unittest\n");
  for (j=0;j<TEST_SIZE;j++) {
    heap_nodes[j].value = rand();
    //heap.push(new HeapNode(rand()));
    heap.push(&(heap_nodes[j]));
  }
  for (j=0;j<TEST_SIZE;j++) {
    //delete heap.pop();
    heap.pop();
  }
  printf("PASS\n");
}
