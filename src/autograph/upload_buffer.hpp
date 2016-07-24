#pragma once
#include <cassert>
#include <mutex>
#include <queue>
#include <vector>
#include "buffer.hpp"

namespace ag {

class upload_buffer {
public:
  upload_buffer(size_t size);
  ~upload_buffer();
  bool upload(const void *data, size_t size, size_t alignment,
              uint64_t expirationDate, buffer_slice &slice);
  bool allocate(uint64_t expirationDate, size_t size, size_t align,
                buffer_slice &slice);
  void reclaim(uint64_t date);

private:
  bool align_offset(size_t align, size_t size, size_t &ptr, size_t space);
  bool tryAllocateContiguousFreeSpace(uint64_t expirationDate, size_t size,
                                      size_t align, size_t &alloc_begin);

  struct fenced_region {
    // device fence
    uint64_t expirationDate;
    // offset to the beginning of the fenced region in the ring buffer
    size_t begin_ptr;
    // offset to the end of the fenced region
    size_t end_ptr;
  };

  // start of free space in the ring
  size_t write_ptr;
  // end of free space in the ring
  size_t begin_ptr;
  // used space
  size_t used;

  buffer buffer;
  void *mapped_region;
  std::queue<fenced_region> fencedRegions;
  std::mutex mutex;
};
}