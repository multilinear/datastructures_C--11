#include <thread>
#include <stdio.h>

#define TSRINGBUFFER_DEBUG
#include "ts_ringbuffer.h"

TSRingBuffer<int> rbuf(150);

void producer() {
  int x;
  for (x=0;x<100;x++) {
    std::chrono::milliseconds dur((100-x));
    std::this_thread::sleep_for(dur);
    rbuf.enqueue(x);
  }
}

void consumer() {
  int x;
  int last=-1;
  for (x=0;x<100;x++) {
    int y;
    bool isnew = false;
    while (!isnew) {
     isnew = rbuf.dequeue(&y);
    }
    if (last+1 != y) {
      PANIC("queue is not queueing!");
    }
    last = y;
    std::chrono::milliseconds dur(y);
    std::this_thread::sleep_for(dur);
  }
}


int main(int argc, char* argv[]) {
  printf("Begin TSRingBuffer.h test\n");
  int i;
  int j;
  int el;

  // *** basic single-threaded tests

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

  // *** single producer, single consumer
  // spawn the consumer
  std::thread consumer_thread(consumer);
  // spawn the producer
  std::thread producer_thread(producer);
  producer_thread.join();
  consumer_thread.join();

  //Test underflow
  if(rbuf.dequeue(&el)) {
    PANIC("underflow returned true");
  }

  //Test overflow
  for (int x=0;x<149;x++) {
    if(!rbuf.enqueue(x)) {
      PANIC("elements overflowing early");
    }
  }
  if(rbuf.enqueue(1)) {
    PANIC("overflow case enqueued anyway?");
  }
  for (int x=0;x<149;x++) {
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
