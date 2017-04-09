/* Copywrite Matthew Brewer 2017-03-22
 *
 * This is a benchmark for dictionaries using external allocation
 * We decide WHAT we're testing using the macro system
 * Our Makefile takes advantage of this.
 *
 * This seems odd, but it avoids code duplication and ensures all our
 * datastructures have the same API (Except where it really isn't a good idea)
 */

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include "panic.h"
#include "timer.h"

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS 1000000
#endif
#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif

#ifdef TEST_AVL
#include "avl.h"
class Node: public AVLNode_base<Node, uint64_t> {
#endif
#ifdef TEST_AVLHASHTABLE
#include "avlhashtable.h"
class Node: public AVLHashTableNode_base<Node, uint64_t> {
#endif
#ifdef TEST_BOUNDEDHASHTABLE
#include "boundedhashtable.h"
class Node: public BoundedHashTableNode_base<Node, uint64_t> {
#endif
#ifdef TEST_DLIST
#include "dlist.h"
class Node: public DListNode_base<Node> {
#endif
#ifdef TEST_OCHASHTABLE
#include "ochashtable.h"
class Node: public OCHashTableNode_base<Node> {
#endif
#ifdef TEST_REDBLACK
#include "redblack.h"
class Node: public RedBlackNode_base<Node, uint64_t> {
#endif 
#ifdef TEST_RREDBLACK
#include "rredblack.h"
class Node: public RRedBlackNode_base<Node, uint64_t> {
#endif 
  public:
    uint64_t value;
  public:
    const uint64_t val(void) const {
      return value;
    }
    static size_t hash(uint64_t v) {
      return v;
    }
    static int compare(const uint64_t v1, const uint64_t v2) {
      if (v1 > v2) return 1;
      if (v1 < v2) return -1;
      return 0;
      //return v1-v2;
    }
    Node() {}
    void set(uint64_t v) {
      value = v;
    }
    Node(uint64_t v) {
      value = v;
    }
};

uint64_t ints[TEST_SIZE];
size_t ints_end;

// necessary for external allocation datastructures
Node *nodes = new Node[TEST_SIZE];

int main(int argc, char* argv[]) {
  #ifdef TEST_OCHASHTABLE
  printf("OCHashTable.h ");
  OCHashTable<Node, uint64_t> hash;
  #endif
  #ifdef TEST_AVLHASHTABLE
  printf("AVLHashTable.h ");
  AVLHashTable<Node, uint64_t> hash;
  #endif
  #ifdef TEST_AVL
  printf("AVL.h ");
  AVL<Node, uint64_t> hash;
  #endif
  #ifdef TEST_BOUNDEDHASHTABLE
  printf("BoundedHashTable.h ");
  BoundedHashTable<Node, uint64_t> hash;
  #endif
  #ifdef TEST_DLIST
  printf("DList.h ");
  DList<Node, uint64_t> hash;
  #endif
  #ifdef TEST_REDBLACK
  printf("RedBlack.h ");
  RedBlack<Node, uint64_t> hash;
  #endif
  #ifdef TEST_RREDBLACK
  printf("RRedBlack.h ");
  RRedBlack<Node, uint64_t> hash;
	#endif 
  printf("test_size=%d test_iterations=%d ", TEST_SIZE, TEST_ITERATIONS); 

  timeb t1, t2;
  ftime(&t1);
  //time_t t1 = time(nullptr);
  size_t j;
  uint64_t get_count=0;
  uint64_t insert_count=0;
  for (j=0; j<TEST_ITERATIONS; j++) {
    ints_end=0;
    size_t i;
    #ifndef USE_MALLOC
    size_t ni=0;
    #endif
    for (i=0; i<TEST_SIZE; i++) {
      bool new_v = false;
      uint64_t r;
      // find a value we haven't used yet
      // We assume collisions are rare enough that the difference between
      // 2 or 100 thousand possible collisions is irrelevent
      // This is why we use 64 bits
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        // assumes RAND_MAX==MAX_INT (it usually does)
        r = rand()*rand();
        new_v = (!hash.get(r)); 
        get_count++;
      }
      insert_count++;
      // put it in thet hash
      #ifdef USE_MALLOC
      Node *n = new Node(); 
      #else
      Node *n = &(nodes[ni++]); 
      #endif
      n->set(r);
      hash.insert(n);
      // and in the list
      ints[ints_end++] = r;
    }
    for(i=0; i<ints_end; i++) {
      uint64_t v = ints[i];
      #ifdef TEST_RREDBLACK
      // RRedblack is weird, it's the only external
      // datastructure we have where there's no advantage
      // to knowing the node for deletion
      hash.get(v); // We still get it, just for equal comparison *shrug*
      hash.remove(v);
      #else
      // Everything else uses the node we get
      auto n = hash.get(v);
      hash.remove(n);
      #endif
      #ifdef USE_MALLOC
      delete n;
      #endif
    }
  }
  ftime(&t2);
  double t = tdiff(t2,t1); 
  //time_t t2 = time(nullptr);
  printf("time=%lf insert=%ld get=%ld\n", t, insert_count, get_count);
}
