#include <thread>
#include "work_queue.h"

WorkQueue<unsigned int> *wq;

void producer() {
  unsigned int x;
  printf("running producer\n");
  for (x=0;x<100;x++) {
    std::chrono::milliseconds dur((100-x));
    std::this_thread::sleep_for(dur);
    wq->enqueue(x);
  }
}

void consumer() {
  unsigned int x;
  printf("running consumer\n");
  for (x=0;x<100;x++) {
    unsigned int y = wq->dequeue();
    printf("%d ", y);
    std::chrono::milliseconds dur(y);
    std::this_thread::sleep_for(dur);
  }
  printf("\n");
}

int main(int argc, char* argv[]) {
  wq = new WorkQueue<unsigned int>();
  // spawn the consumer
  std::thread consumer_thread(consumer);
  // spawn the producer
  std::thread producer_thread(producer);
  producer_thread.join();
  consumer_thread.join(); 
  printf("Done! all ints should be in order\n");
}
