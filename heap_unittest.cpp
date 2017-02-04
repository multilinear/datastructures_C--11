#define HEAP_DEBUG
#include "heap.h"
#define ARRAY_DEBUG
#include "array.h"
#define DELAYED_COPY_ARRAY_DEBUG
#include "delayed_copy_array.h"

#define TEST_SIZE 200

class HeapCompare {
  public:
    static int compare(int *v1, int *v2) {
      return (*v1) - (*v2);
    }
};

void print_heap(Heap<DCUArray<int>, int, HeapCompare> *heap) {
  printf("[");
  for (size_t i=0; i<heap->len(); ++i) {
    printf("%d ", heap->get(i));
  }
  printf("]\n");
}

int main(int argc, char **argv) {
  printf("Begin Heap.h unittest\n");
  //Heap<UArray<int>, int, HeapCompare> heap;
  Heap<DCUArray<int>, int, HeapCompare> heap;
  int i,j;
  int val=-1;
  for (j=TEST_SIZE-1;j<TEST_SIZE;j++) {
    j=TEST_SIZE;
    for (i=0;i<j;i++) {
      //printf("push %d\n", i);
      heap.push(i);
      //print_heap(&heap);
    }
    for (i=0;i<j;i++) {
      heap.pop(&val);
      //printf("pop %d\n", val);
      //print_heap(&heap);
    }
    if (heap.pop(&val)) {
      PANIC("Bheap didn't drain");
    }
    for (i=j;i>0;i--) {
      //printf("push %d\n", i);
      heap.push(i);
      //print_heap(&heap);
    }
    for (i=j;i>0;i--) {
      heap.pop(&val);
      //printf("popped %d\n", val);
      //print_heap(&heap);
      heap.check();
    }
    if (heap.pop(&val)) {
      PANIC("Bheap didn't drain");
    }
  }
  for (j=0;j<TEST_SIZE;j++) {
    for (i=0;i<j;i++) {
      val = rand();
      //printf("push %d\n", val);
      heap.push(val);
      //print_heap(&heap);
    }
    for (i=0;i<j;i++) {
      heap.pop(&val);
      //printf("popped %d\n", val);
      //print_heap(&heap);
    }
    if (heap.pop(&val)) {
      PANIC("Bheap didn't drain");
    }
  }

  // Testing duplicate
  for (i=0; i<10; i++) {
    heap.push(1);
  }
  for (i=0; i<10; i++) {
    if (!heap.pop(&val)) {
      PANIC("duplicate handling broken"); 
    }
  }
  if (heap.pop(&val)) {
    PANIC("Bheap didn't drain");
  }


  printf("PASS\n");
  return 0;
}
