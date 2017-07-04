#pragma once
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Exports.h>
#include <cassert>
#include <mutex>
#include <queue>
#include <vector>

namespace ag {

class AG_GFX_API UploadBuffer {
public:
  UploadBuffer() = default;
  UploadBuffer(size_t size);

  bool upload(const void *data, size_t size, size_t alignment,
              uint64_t expirationDate, Buffer::Slice &slice);
  bool allocate(uint64_t expirationDate, size_t size, size_t align,
                Buffer::Slice &slice);
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
  size_t write_ptr = 0;
  // end of free space in the ring
  size_t begin_ptr = 0;
  // used space
  size_t used = 0;
  void *mapped_region = nullptr;
  std::queue<fenced_region> fencedRegions;
  std::mutex mutex;
};

// Upload data to the default upload buffer
AG_GFX_API Buffer::Slice uploadFrameData(const void *data, size_t size,
                                         size_t alignment = -1);

template <typename T>
inline Buffer::Slice uploadFrameArray(const T *data, size_t num_elements,
                                      size_t alignment = -1) {
  return uploadFrameData(data, num_elements * sizeof(T), alignment);
}

template <typename T, size_t N>
inline Buffer::Slice uploadFrameArray(const std::array<T, N> &data,
                                      size_t alignment = -1) {
  return uploadFrameData(data.data(), N * sizeof(T), alignment);
}

template <typename T, size_t N>
inline Buffer::Slice uploadFrameArray(T (&data)[N], size_t alignment = -1) {
  return uploadFrameData(&data[0], N * sizeof(T), alignment);
}

} // namespace ag