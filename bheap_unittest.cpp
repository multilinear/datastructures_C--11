//#define BHEAP_DEBUG
#define ARRAY_DEBUG
#define BHEAP_DEBUG
//#define BHEAP_DEBUG_VERBOSE
#include "bheap.h"

#define TEST_SIZE 70 
// TODO: Make sure this works with not *just* 2^x-1
#define NODE_SIZE 3

class BHeapCompare {
  public:
    static int val(int v) {
      return v;
    }
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
};

int main(int argc, char **argv) {
  printf("Begin BHeap.h/Array.h unittest\n");
  BHeap<int, int, BHeapCompare, NODE_SIZE> heap;
  printf("Constructed \n");
  int i,j;
  int val=-1;
  //for (j=TEST_SIZE-1;j<TEST_SIZE;j++) {
  j=TEST_SIZE;
    for (i=0;i<j;i++) {
      heap.push(i);
      heap.print();
      printf("\n");
    }
    for (i=0;i<j;i++) {
      heap.pop(&val);
      heap.print();
      printf("\n");
    }
/*    heap.print();
    for (i=j;i>0;i--) {
      heap.push(i);
    }
    heap.print();
    for (i=j;i>0;i--) {
      heap.pop(&val);
    }
    heap.print();*/
  //}
/*  for (j=0;j<TEST_SIZE;j++) {
    for (i=0;i<j;i++) {
      val = rand();
      heap.push(val);
    }
    heap.print();
    for (i=0;i<j;i++) {
      heap.pop(&val);
    }
    heap.print();
  }*/

  printf("PASS\n");
  return 0;
}
