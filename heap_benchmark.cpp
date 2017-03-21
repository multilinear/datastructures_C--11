#include "heap.h"
#include "array.h"

#define TEST_SIZE 100
#define PASSES 1000000

#ifdef TEST_BHEAP
#ifndef NODE_SIZE
#define NODE_SIZE 3
#endif
#include "bheap.h"
#endif

#ifdef TEST_BOUNDEDHEAP
#include "boundedheap.h"
class HeapNode: public BoundedHeapNode_base<HeapNode, int>{
  public:
    int value;
    HeapNode() {};
    HeapNode(int val) { value=val; }
    int val() {
      return value;
    }
		void set(int v) {
			value = v;
		}		
    static int compare(int v1, int v2) {
      return v1 - v2;
    }
};
#endif

#ifdef TEST_HEAP_DICTARRAY
#include "dictarray.h"
#endif

#ifdef TEST_HEAP_DCARRAY
#include "delayed_copy_array.h"
#endif

#ifdef TEST_HEAP_TREEARRAY
#include "treearray.h"
#endif

class HeapCompare {
  public:
    static int val(int v) {
      return v;
    }
    static int compare(int *v1, int *v2) {
      return *v1 - *v2;
    }
};

int main(int argc, char **argv) {
  #ifdef TEST_BHEAP
  printf("Begin BHeap benchmark\n");
  BHeap<int, HeapCompare, NODE_SIZE> heap;
  #endif
  #ifdef TEST_BOUNDEDHEAP
  printf("Begin BoundedHeap benchmark\n");
  BoundedHeap<HeapNode, int> heap;
	HeapNode heap_nodes[TEST_SIZE];
  #endif
  #ifdef TEST_HEAP_DICTARRAY
  printf("Begin Heap on dict array benchmark\n");
  Heap<DictUArray<int>, int, HeapCompare> heap;
  #endif
  #ifdef TEST_HEAP_DCARRAY
  printf("Begin Heap on delayed copy array benchmark\n");
  Heap<DCUArray<int>, int, HeapCompare> heap;
  #endif
  #ifdef TEST_HEAP_TREEARRAY
  printf("Begin Heap on tree array benchmark\n");
  Heap<TreeUArray<int, 12>, int, HeapCompare> heap;
  #endif
  #ifdef TEST_HEAP_UARRAY
  printf("Begin Heap on doubling array benchmark\n");
  Heap<UArray<int>, int, HeapCompare> heap;
  #endif

  int j;

  for (int i=0; i<PASSES; i++) {
		#ifdef TEST_BOUNDEDHEAP
		#ifndef USE_MALLOC
		int ni=0;
		#endif
    #endif
    for (j=0;j<TEST_SIZE;j++) {
			#ifdef TEST_BOUNDEDHEAP
			#ifndef USE_MALLOC
			auto n = &(heap_nodes[ni++]);
			#else
			auto n = new HeapNode();
			#endif
			n->set(rand());
			heap.push(n);
			#else
      heap.push(rand());
			#endif
    }
    for (j=0;j<TEST_SIZE;j++) {
			#ifdef TEST_BOUNDEDHEAP
			#ifndef USE_MALLOC
      heap.pop();
			#else
      delete heap.pop();
			#endif
			#else
      int val=-1;
      heap.pop(&val);
			#endif
    }
  }

  printf("Done\n");
  return 0;
}
