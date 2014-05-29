/* Copyright: Matthew Brewer (mbrewer@smalladventures.net) 2014-04-22
 * 
 * Mostly Threadsafe lockless ringbuffer implementation with guaranteed
 * progress.
 *
 * Threading Details:
 *  "mostly" threadsafe is due to a restriction to single writer.
 *  With single-writer/single-reader this code is completely wait-free 
 * 
 *  In single-writer/multi-reader the readers may spin against other's beating
 *  it to the data, and thus in high contention cases we can get "waiting"
 *  behavior, but we always maintain progress.
 * 
 * Possible races:
 *  Like any ring buffer if one thread gets stalled until tail or head is able
 *  to make it ALL the way around the buffer (requiring both tail and head to
 *  move), we could get a race where e.g. we over-write data in the buffer.
 *  Note that this race only occurs if both head and tail move under a thread,
 *  thus it can  only. Happen when multiple readers are present.
 *
 * Use:
 *  instantiate and enqueue, nothing hard here.
 * 
 */


#include <atomic>
#include "panic.h"

#ifndef TSRINGBUFFER_H
#define TSRINGBUFFER_H

template<typename T>
class TSRingBuffer {
  private:
    std::atomic<T>* buf;
    size_t buf_size;
    // data comes out here
    std::atomic<size_t> head;
    // data goes in here
    std::atomic<size_t> tail;
  public:
    TSRingBuffer(size_t buf_size);
    ~TSRingBuffer();
    bool enqueue(T data);
    bool dequeue(T* data);
};

template<typename T>
TSRingBuffer<T>::TSRingBuffer(size_t size) {
  buf_size = size;
  buf = new std::atomic<T>[size];
  head = 0;
  tail = 0;
}

template<typename T>
TSRingBuffer<T>::~TSRingBuffer() {
  if (head != tail) {
    PANIC("TSRingBuffer destroyed with elements in it\n");
  }
  delete buf; 
}

/* Note, this is only safe for 1 writer!
 * 
 * We put data in, and then we increment tail.
 * If someone else succeeds at the same operation, then we can corrupt the data
 * that they have written. We'll know it, but there's not much we can do.
 */
template<typename T>
bool TSRingBuffer<T>::enqueue(T data) {
  // Because this is only safe for one writer anyway, we can elide the loop
  // we're the only thread that can modify tail anyway
  // Tail still needs to be atomic though, because we READ it in other threads
  // to make sure we only pull out data that's actually there.

  // Note: if we stall somewhere in here until tail loops all the way around
  // and comes back (requiring head to move), we'll race and break.
  // Hopefully that's unlikely to happen, but keep it in mind.
  auto old_tail = tail.load();
  auto new_tail = (old_tail+1) % buf_size;
  if (new_tail == head.load()) {
    // this indicates the buffer is full.
    // this is racey, but if it says "yes" it's safe, and if it says "no" it
    // was *almost* not safe.
    return false; 
  }
  buf[old_tail].store(data);
  // Note this can mutate new_tail!
  // Because we don't have a loop we can just use a normal store
  tail.store(new_tail); 
  return true;
}

template<typename T>
bool TSRingBuffer<T>::dequeue(T* data) {
  bool dequeued = false;
  // Note
  // - if we lose the race below, SOMEONE won. Thus this is guaranteed
  // progress, but not wait-free.
  // - additionally that contention only occurs with multiple readers
  // - if we stall somewhere in here until head loops all the way around
  // (requiring tail to move) and comes back, we'll race and break.
  // Hopefully that's unlikely to happen, but keep it in mind.
  while(!dequeued) {
    size_t old_head;
    size_t new_head;
    old_head = head.load();
    if (old_head == tail.load()) {
      // This indicates the buffer is empty.
      // this is racy, but if it says "yes" it's safe, and if it says "no" it
      // was "almost" not safe.
      return false;
    }
    *data = buf[head].load();
    new_head = (old_head+1) % buf_size;
    dequeued = head.compare_exchange_weak(old_head, new_head);
  }
  return true;
}

#endif
