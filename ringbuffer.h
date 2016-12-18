#include "panic.h"
#include "array.h"

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

template<typename T, size_t BUFF_SIZE>
class RingBuffer {
  private:
    StaticArray<T, BUFF_SIZE> buf;
    // data comes out here
    size_t head;
    // data goes in here
    size_t tail;
  public:
    RingBuffer();
    ~RingBuffer();
    bool enqueue(T data);
    bool dequeue(T* data);
};

template<typename T, size_t BUFF_SIZE>
RingBuffer<T, BUFF_SIZE>::RingBuffer() {
  head = 0;
  tail = 0;
}

template<typename T, size_t BUFF_SIZE>
RingBuffer<T, BUFF_SIZE>::~RingBuffer() {
  if (head != tail) {
    PANIC("RingBuffer destroyed with elements in it\n");
  }
}

template<typename T, size_t BUFF_SIZE>
bool RingBuffer<T, BUFF_SIZE>::enqueue(T data) {
  if ((tail + 1) % BUFF_SIZE == head) {
    return false; 
  }
  buf[tail] = data;
  tail = (tail + 1) % BUFF_SIZE;
  return true;
}

template<typename T, size_t BUFF_SIZE>
bool RingBuffer<T, BUFF_SIZE>::dequeue(T* data) {
  if (head == tail) {
    return false;
  }
  *data = buf[head];
  head = (head + 1) % BUFF_SIZE;
  return true;
}

#endif
