#include <autograph/gl/UploadBuffer.h>
#include <algorithm>
#include <cstring>

namespace ag {
namespace gl {
// A ring buffer, used in the implementation of upload buffers
UploadBuffer::UploadBuffer(size_t size)
    : buffer_{size, BufferUsage::Upload}, write_ptr(0),
      begin_ptr(0), used(0) {
  mapped_region = buffer_.map(0, size);
}

UploadBuffer::~UploadBuffer() {}

bool UploadBuffer::upload(const void *data, size_t size, size_t alignment,
                          uint64_t expirationDate, BufferSlice &slice) {
  if (!allocate(expirationDate, size, alignment, slice))
    return false;
  // copy data
  memcpy((char *)mapped_region + slice.offset, data, size);
  return true;
}

bool UploadBuffer::allocate(uint64_t expirationDate, size_t size, size_t align,
                            BufferSlice &slice) {
  size_t offset = 0;
  if (!tryAllocateContiguousFreeSpace(expirationDate, size, align, offset))
    return false;
  slice.obj = buffer_.object();
  slice.offset = offset;
  slice.size = size;
  return true;
}

bool UploadBuffer::alignOffset(size_t align, size_t size, size_t &ptr,
                               size_t space) {
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

bool UploadBuffer::tryAllocateContiguousFreeSpace(uint64_t expirationDate,
                                                  size_t size, size_t align,
                                                  size_t &alloc_begin) {
  std::lock_guard<std::mutex> guard(mutex);
  assert(size < buffer_.size());
  if ((begin_ptr < write_ptr) || ((begin_ptr == write_ptr) && (used == 0))) {
    size_t slack_space = buffer_.size() - write_ptr;
    // try to put the buffer in the slack space at the end
    if (!alignOffset(align, size, write_ptr, slack_space)) {
      // else, try to put it at the beginning (which is always correctly
      // aligned)
      if (size > begin_ptr)
        return false;
      write_ptr = 0;
    }
  } else { // begin_ptr > write_ptr
    // reclaim space in the middle
    if (alignOffset(align, size, write_ptr, begin_ptr - write_ptr))
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

void UploadBuffer::reclaim(uint64_t date) {
  while (!fencedRegions.empty() &&
         fencedRegions.front().expirationDate <= date) {
    auto &r = fencedRegions.front();
    begin_ptr = r.end_ptr; // there may be some alignment space that we would
                           // want to reclaim
    used -= r.end_ptr - r.begin_ptr;
    fencedRegions.pop();
  }
}
}
}