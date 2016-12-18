#include "queue.h"

int main(int argc, char* argv[]) {
  printf("Begin Queue.h test\n");
  Queue<int> q;
  int i;
  for (int i=0; i<10; i++) {
    q.enqueue(i);
  }
  i = 0;
  for (auto j = q.begin(); j != q.end(); ++j) {
    if (*j != i) {
      PANIC("iterator is broken");
    }
    i++;
  }
  if (i != 10) {
    PANIC("iterator is broken");
  }

  int val;
  for (int i=0; i<10; i++) {
    if (!q.dequeue(&val)) {
      PANIC("queue is empty when it shouldn't be");
    }
  }
  if (q.dequeue(&i)) {
    PANIC("queue is empty when it shouldn't be");
  }
  printf("PASS\n"); 
}
