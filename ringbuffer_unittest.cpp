#include <stdio.h>

#define RINGBUFFER_DEBUG
#include "ringbuffer.h"

int main(int argc, char* argv[]) {
  printf("Begin RingBuffer.h test\n");
  RingBuffer<int> rbuf(13);
  int i;
  int j;
  int el = 0;
  // fill and empty at different numbers
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      rbuf.enqueue(j);
    }
    for (j = 0; j<i; j++) {
      rbuf.dequeue(&el);
    }
  }
  // add an element
  rbuf.enqueue(-1);
  // now put elements in and pull them out, but leaving one
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      rbuf.enqueue(j);
    }
    for (j = 0; j<i; j++) {
      rbuf.dequeue(&el);
    }
  }
  // add another element
  rbuf.enqueue(-1);
  // now put elements in and pull them out, but leaving two
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      rbuf.enqueue(j);
    }
    for (j = 0; j<i; j++) {
      rbuf.dequeue(&el);
    }
  }
  rbuf.dequeue(&el);
  if (el != 7) {
    PANIC("RingBuffer is not acting like a queue");
  }
  rbuf.dequeue(&el);
  if (el != 8) {
    PANIC("RingBuffer is not acting like a queue");
  }
  printf("PASS\n");
  // And test destructor here
  return 0;
}
