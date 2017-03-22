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
#include "panic.h"

#define TEST_ITERATIONS 1000000
#define TEST_SIZE 100

#ifdef TEST_AVL
#include "avl.h"
class Node: public AVLNode_base<Node, int> {
#endif
#ifdef TEST_AVLHASHTABLE
#include "avlhashtable.h"
class Node: public AVLHashTableNode_base<Node, int> {
#endif
#ifdef TEST_OCHASHTABLE
#include "ochashtable.h"
class Node: public OCHashTableNode_base<Node> {
#endif
#ifdef TEST_REDBLACK
#include "redblack.h"
class Node: public RedBlackNode_base<Node, int> {
#endif 
#ifdef TEST_RREDBLACK
#include "rredblack.h"
class Node: public RRedBlackNode_base<Node, int> {
#endif 
  public:
    int value;
  public:
    const int val(void) const {
      return value;
    }
    static size_t hash(int v) {
      return v;
    }
    static int compare(const int v1, const int v2) {
      return v1-v2;
    }
    Node() {}
    void set(int v) {
      value = v;
    }
    Node(int v) {
      value = v;
    }
};

int ints[TEST_SIZE];
int ints_end;

// necessary for external allocation datastructures
Node *nodes = new Node[TEST_SIZE];

int main(int argc, char* argv[]) {
  #ifdef TEST_OCHASHTABLE
  printf("Begin OCHashTable.h benchmark\n");
  OCHashTable<Node, int> hash;
  #endif
  #ifdef TEST_AVLHASHTABLE
  printf("Begin AVLHashTable.h benchmark\n");
  AVLHashTable<Node, int> hash;
  #endif
  #ifdef TEST_AVL
  printf("Begin AVL.h benchmark\n");
  AVL<Node, int> hash;
  #endif
  #ifdef TEST_REDBLACK
  printf("Begin RedBlack.h benchmark\n");
  RedBlack<Node, int> hash;
  #endif
  #ifdef TEST_RREDBLACK
  printf("Begin RRedBlack.h benchmark\n");
  RRedBlack<Node, int> hash;
	#endif 

  int j;
  int get_count=0;
  for (j=0; j<TEST_ITERATIONS; j++) {
    ints_end=0;
    int i;
    #ifndef USE_MALLOC
    int ni=0;
    #endif
    for (i=0; i<TEST_SIZE; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = (!hash.get(r)); 
        get_count++;
      }
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
      int v = ints[i];
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
  printf("get_count %d\n", get_count);
}
