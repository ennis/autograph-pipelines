#pragma once
#include <autograph/gl/Config.h>
#include <autograph/gl/Buffer.h>
#include <cassert>
#include <mutex>
#include <queue>
#include <vector>

namespace ag {

class AG_GL_API UploadBuffer {
public:
  UploadBuffer(size_t size);
  ~UploadBuffer();
  bool upload(const void *data, size_t size, size_t alignment,
              uint64_t expirationDate, BufferSlice &slice);
  bool allocate(uint64_t expirationDate, size_t size, size_t align,
                BufferSlice &slice);
  void reclaim(uint64_t date);

private:
  bool alignOffset(size_t align, size_t size, size_t &ptr, size_t space);
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

  Buffer buffer_;
  // start of free space in the ring
  size_t write_ptr;
  // end of free space in the ring
  size_t begin_ptr;
  // used space
  size_t used;
  void *mapped_region;
  std::queue<fenced_region> fencedRegions;
  std::mutex mutex;
};
}