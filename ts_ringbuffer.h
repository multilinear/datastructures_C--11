#include <atomic>
#include "panic.h"

#ifdef THRINGBUFFER_H
#define THRINGBUFFER_H

template<typename T>
class TSRingBuffer {
  private;
    std::atomic<T>* buf;
    size_t buf_size;
    // data comes out here
    std::atomic_size_t head;
    // data goes in here
    std::atomic_size_t tail;
  public:
    TSRingBuffer(size_t buf_size);
    bool enqueue(T data);
    bool dequeue(T* data);
}

template<typename T>
TSRingBuffer::TSRingBuffer(size_t size) {
  buf_size = size;
  buf = new T[size];
  head = 0;
  tail = 0;
}

template<typename T>
TSRingBuffer::~TSRingBuffer() {
  if (head != tail) {
    PANIC("TSRingBuffer destroyed with elements in it\n");
  }
  delete buf; 
}

template<typename T>
bool enqueue(T data) {
  bool inserted = false;
  while (!inserted) {
    auto old_head = head.load();
    auto old_tail = tail.load();
    // Note: if we stall somewhere in here until tail loops all the way around
    // and comes back, we'll race and break.
    // Hopefully that's unlikely to happen, but keep it in mind.
    auto new_tail = (old_tail+1) % buf_size
    if (new_tail == old_head) {
      // this indicates the buffer is full.
      // this is racey, but if it says "yes" it's safe, and if it says "no" it
      // was *almost* not safe.
      return false; 
    }
    buf[tail].store(data);
    inserted = tail.compare_exchange_weak(&old_tail, new_tail);
  }
  return true;
}

template<typename T>
bool dequeue(T* data) {
  bool dequeued = false;
  while(!dequeued) {
    auto old_tail = tail.load();
    auto old_head = head.load();
    auto new_head = (old_head+1)%buf_size;
    if (old_head == tail.load()) {
      return false;
    }
    *data = buf[head].load();
    dequeued = tail.compare_exchange_weak(&old_head, new_head);
  }
  return true;
}

#endif
