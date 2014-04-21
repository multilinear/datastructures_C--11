#include "panic.h"

#ifdef RINGBUFFER_H
#define RINGBUFFER_H

template<typename T>
class RingBuffer {
  private;
    T* buf;
    size_t buf_size;
    // data comes out here
    size_t head;
    // data goes in here
    size_t tail;
  public:
    RingBuffer(size_t buf_size);
    bool enqueue(T data);
    bool dequeue(T* data);
}

RingBuffer::RingBuffer(size_t size) {
  buf_size = size;
  buf = new T[size];
  head = 0;
  tail = 0;
}

RingBuffer::~RingBuffer() {
  if (head != tail) {
    PANIC("RingBuffer destroyed with elements in it\n");
  }
  delete buf; 
}

bool enqueue(T data) {
  if ((tail + 1) % buf_size == head) {
    return false; 
  }
  buf[tail] = data;
  tail = (tail + 1) % buf_size;
  return true;
}

bool dequeue(T* data) {
  if (head == tail) {
    return false;
  }
  *data = buf[head];
  head = (head + 1) % buf_size;
  return true;
}

#endif
