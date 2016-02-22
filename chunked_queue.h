#ifndef BASE_CHUNKED_QUEUE_H_
#define BASE_CHUNKED_QUEUE_H_

#include <algorithm>
#include <memory>
#include "base/logging.h"

namespace c3d {
template <typename T>
constexpr int ChunkedQueueDefaultChunkSize() {
  return ((1024 / sizeof(T)) <= 4) ? 4 : (1024 / sizeof(T));
}

// Fast FIFO queue.
template <typename T, int INLINED_SIZE = (2 + 64 / sizeof(T)),
          int CHUNK_SIZE = ChunkedQueueDefaultChunkSize<T>()>
class ChunkedQueue {
public:
  ChunkedQueue();
#if 0
  ChunkedQueue(ChunkedQueue&& other);
  operator=(ChunkedQueue&& other);
#endif

  void push_back(const T& arg) { emplace_back(arg); }
  void push_back(T&& arg) { emplace_back(std::move(arg)); }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (size_ < INLINED_SIZE) {
      inlined_[size_++] = T(std::forward<Args>(args)...);
      return;
    }
    if (first_ == nullptr) {
      first_.reset(new Chunk);
      last_ = first_.get();
    } else if (last_->chunk_size >= CHUNK_SIZE) {
      auto* new_chunk = new Chunk;
      new_chunk->prev = last_;
      last_->next.reset(new_chunk);
      last_ = new_chunk;
    }
    last_->data[last_->chunk_size++] = T(std::forward<Args>(args)...);
    ++size_;
  }

  void pop_back();

  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  const T& back() const {
    DCHECK_GT(size_, 0);
    if (size_ < INLINED_SIZE) {
      return inlined_[size_ - 1];
    }
    return last_->data[last_->chunk_size - 1];
  }

private:
  struct Chunk {
    T data[CHUNK_SIZE];
    int chunk_size = 0;
    std::unique_ptr<Chunk> next;
    Chunk* prev = nullptr;
  };
  size_t size_;
  T inlined_[INLINED_SIZE];
  std::unique_ptr<Chunk> first_;
  Chunk* last_;  // Not owned
};

#define BOILERPLATE \
  template <typename T, int INLINED_SIZE, int CHUNK_SIZE>

BOILERPLATE ChunkedQueue<T, INLINED_SIZE, CHUNK_SIZE>::ChunkedQueue()
    : size_(0), last_(nullptr) {}

BOILERPLATE void ChunkedQueue<T, INLINED_SIZE, CHUNK_SIZE>::pop_back() {
  --size_;

  if (last_ != nullptr) {
    if (last_->chunk_size > 0) {
      --last_->chunk_size;
      return;
    }
    Chunk* to_delete = last_;
    last_ = last_->prev;
    if (last_ != nullptr) {
      DCHECK(last_->next.get() == to_delete);
      last_->next.reset();
    } else {
      DCHECK(first_.get() == to_delete);
      first_.reset();
    }
  }
}

#undef BOILERPLATE

}  // namespace c3d
#endif  // BASE_CHUNKED_QUEUE_H_
