#include <stdio.h>

#define RINGBUFFER_DEBUG
#include "ringbuffer.h"

int main(int argc, char* argv[]) {
  printf("Begin RingBuffer.h test\n");
  RingBuffer<int, 13> rbuf;
  int i;
  int j;
  int el = 0;
  // fill and empty at different numbers
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      if (!rbuf.enqueue(j)) {
        PANIC("ringbuffer enqueue failed");
      }
    }
    for (j = 0; j<i; j++) {
      if (!rbuf.dequeue(&el)) {
        PANIC("ringbuffer dequeue failed");
      }
    }
  }
  // add an element
  rbuf.enqueue(-1);
  // now put elements in and pull them out, but leaving one
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      if (!rbuf.enqueue(j)) {
        PANIC("ringbuffer enqueue failed");
      }
    }
    for (j = 0; j<i; j++) {
      if (!rbuf.dequeue(&el)) {
        PANIC("ringbuffer dequeue failed");
      }
    }
  }
  // add another element
  rbuf.enqueue(-1);
  // now put elements in and pull them out, but leaving two
  for (i = 0; i<10; i++) { 
    for (j = 0; j<i; j++) {
      if(!rbuf.enqueue(j)) {
        PANIC("ringbuffer enqueue failed");
      }
    }
    for (j = 0; j<i; j++) {
      if(!rbuf.dequeue(&el)) {
        PANIC("ringbuffer dequeue failed");
      }
    }
  }
  if(!rbuf.dequeue(&el)) {
    PANIC("ringbuffer dequeue failed");
  }
  if (el != 7) {
    PANIC("RingBuffer is not acting like a queue");
  }
  if(!rbuf.dequeue(&el)) {
    PANIC("ringbuffer dequeue failed");
  }
  if (el != 8) {
    PANIC("RingBuffer is not acting like a queue");
  }

  //Test underflow
  if(rbuf.dequeue(&el)) {
    PANIC("underflow returned true");
  }

  //Test overflow
  for (int x=0;x<12;x++) {
    if(!rbuf.enqueue(x)) {
      PANIC("elements overflowing early");
    }
  }
  if(rbuf.enqueue(13)) {
    PANIC("overflow case enqueued anyway?"); 
  }
  for (int x=0;x<12;x++) {
    if(!rbuf.dequeue(&el)) {
      PANIC("some elements missing");
    }
    if (el!=x) {
      PANIC("data corruption");
    }
  }
  if(rbuf.dequeue(&el)) {
    PANIC("empty list returned sometihng anyway"); 
  }


  printf("PASS\n");
  // And test destructor here
  return 0;
}
