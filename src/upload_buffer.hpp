#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <algorithm>
#include <cassert>
#include <mutex>
#include <queue>
#include <vector>

#include "gl_buffer.hpp"

// A ring buffer, used in the implementation of upload buffers
class upload_buffer {
public:
  upload_buffer(size_t size)
      : buffer{gl_buffer::create(size, gl_buffer_usage::upload)}, write_ptr(0),
        begin_ptr(0), used(0) {
    mapped_region = nullptr; // TODO
  }

  ~upload_buffer() {}

  bool upload(const void *data, size_t size, size_t alignment,
              uint64_t expirationDate, gl_buffer_slice &slice) {
    if (!allocate(expirationDate, size, alignment, slice))
      return false;
    // copy data
    memcpy((char *)mapped_region + slice.offset, data, size);
    return true;
  }

  bool allocate(uint64_t expirationDate, size_t size, size_t align,
                gl_buffer_slice &slice) {
    size_t offset = 0;
    if (!tryAllocateContiguousFreeSpace(expirationDate, size, align, offset))
      return false;
    slice.obj = buffer.object();
    slice.offset = offset;
    slice.size = size;
    return true;
  }

  bool align_offset(size_t align, size_t size, size_t &ptr, size_t space) {
    size_t off = ptr & (align - 1);
    if (off > 0)
      off = align - off;
    if (space < off || space - off < size)
      return false;
    else {
      ptr = ptr + off;
      return true;
    }
  }

  bool tryAllocateContiguousFreeSpace(uint64_t expirationDate, size_t size,
                                      size_t align, size_t &alloc_begin) {
    std::lock_guard<std::mutex> guard(mutex);
    assert(size < buffer.byte_size_);
    if ((begin_ptr < write_ptr) || ((begin_ptr == write_ptr) && (used == 0))) {
      size_t slack_space = buffer.byte_size_ - write_ptr;
      // try to put the buffer in the slack space at the end
      if (!align_offset(align, size, write_ptr, slack_space)) {
        // else, try to put it at the beginning (which is always correctly
        // aligned)
        if (size > begin_ptr)
          return false;
        write_ptr = 0;
      }
    } else { // begin_ptr > write_ptr
      // reclaim space in the middle
      if (align_offset(align, size, write_ptr, begin_ptr - write_ptr))
        alloc_begin = write_ptr;
      else
        return false;
    }

    alloc_begin = write_ptr;
    used += size;
    write_ptr += size;
    fencedRegions.push(
        fenced_region{expirationDate, alloc_begin, alloc_begin + size});
    return true;
  }

  void reclaim(uint64_t date) {
    while (!fencedRegions.empty() &&
           fencedRegions.front().expirationDate <= date) {
      auto &r = fencedRegions.front();
      begin_ptr = r.end_ptr; // there may be some alignment space that we would
                             // want to reclaim
      used -= r.end_ptr - r.begin_ptr;
      fencedRegions.pop();
    }
  }

private:
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

  gl_buffer buffer;
  void *mapped_region;
  std::queue<fenced_region> fencedRegions;
  std::mutex mutex;
};

#endif // !RING_BUFFER_HPP
