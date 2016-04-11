#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdlib> // size_t
#include "gl_handle.hpp"

namespace ag {
enum class BufferUsage {
  Upload,  // CPU-visible, write-only
  Default, // GPU-visible, cannot be accessed by the CPU
  Readback // CPU-visible, read-only
};

struct buffer_deleter {
    void operator()(GLuint obj) {
      gl::DeleteBuffers(1, &obj);
    }
  };

struct RawBuffer {
  RawBuffer() = default;
  RawBuffer(std::size_t byteSize_, BufferUsage usage_, GLuint obj_)
      : usage(usage_), byteSize(byteSize_), obj(obj_) {}



    BufferUsage usage;
  std::size_t byteSize;
  gl_handle<buffer_deleter> obj;
};

template <typename T> struct Buffer : public RawBuffer {
  Buffer() = default;
  Buffer(BufferUsage usage_, GLuint obj_)
      : RawBuffer(sizeof(T), usage_, obj_) {}
};

// specialization for array types
template <typename T> struct Buffer<T[]> : public RawBuffer {
  Buffer() = default;
  Buffer(BufferUsage usage_, std::size_t size_, GLuint obj_)
      : RawBuffer(size_ * sizeof(T), usage_, obj_) {}

  constexpr std::size_t size() const {
    // must use this pointer to make byteSize a dependent name
    return this->byteSize / sizeof(T);
  }
};

// A slice of a buffer (untyped)
struct RawBufferSlice {
  RawBufferSlice() : obj(0), offset(0), byteSize(0) {}

  RawBufferSlice(GLuint obj_, size_t offset_,
                 size_t byteSize_)
      : obj(obj_), offset(offset_), byteSize(byteSize_) {}

  GLuint obj;
  size_t offset;
  size_t byteSize;
};

template <typename T>
struct BufferSlice : public RawBufferSlice {
  BufferSlice(GLuint obj_, size_t offset_)
      : RawBufferSlice(obj_, offset_, sizeof(T)) {}
};

// specialization for array types
template <typename T>
struct BufferSlice<T[]> : public RawBufferSlice {
  BufferSlice(GLuint obj_, size_t offset_, size_t size_)
      : RawBufferSlice(obj_, offset_, size_ * sizeof(T)) {}

  constexpr std::size_t size() const { return this->byteSize / sizeof(T); }
};

}

#endif
