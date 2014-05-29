#include <thread>
#include <stdio.h>
#include "ts_work_queue.h"

WorkQueue<int> *wq;

void producer() {
  int x;
  for (x=0;x<100;x++) {
    std::chrono::milliseconds dur((100-x));
    std::this_thread::sleep_for(dur);
    wq->enqueue(x);
  }
}

void consumer() {
  int x;
  int last = -1;
  for (x=0;x<100;x++) {
    int y = wq->dequeue();
    if (last + 1 != y) {
      PANIC("workQueue is not acting like a queue");
    }
    last = y;
    std::chrono::milliseconds dur(y);
    std::this_thread::sleep_for(dur);
  }
}

int main(int argc, char* argv[]) {
  printf("Begin TSWorkQueue.h unittest\n");
  wq = new WorkQueue<int>();
  // spawn the consumer
  std::thread consumer_thread(consumer);
  // spawn the producer
  std::thread producer_thread(producer);
  producer_thread.join();
  consumer_thread.join(); 
  printf("PASS\n");
}
