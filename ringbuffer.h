#include "panic.h"

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

template<typename T>
class RingBuffer {
  private:
    T* buf;
    size_t buf_size;
    // data comes out here
    size_t head;
    // data goes in here
    size_t tail;
  public:
    RingBuffer(size_t buf_size);
    ~RingBuffer();
    bool enqueue(T data);
    bool dequeue(T* data);
};

template<typename T>
RingBuffer<T>::RingBuffer(size_t size) {
  buf_size = size;
  buf = new T[size];
  head = 0;
  tail = 0;
}

template<typename T>
RingBuffer<T>::~RingBuffer() {
  if (head != tail) {
    PANIC("RingBuffer destroyed with elements in it\n");
  }
  delete buf; 
}

template<typename T>
bool RingBuffer<T>::enqueue(T data) {
  if ((tail + 1) % buf_size == head) {
    return false; 
  }
  buf[tail] = data;
  tail = (tail + 1) % buf_size;
  return true;
}

template<typename T>
bool RingBuffer<T>::dequeue(T* data) {
  if (head == tail) {
    return false;
  }
  *data = buf[head];
  head = (head + 1) % buf_size;
  return true;
}

#endif
