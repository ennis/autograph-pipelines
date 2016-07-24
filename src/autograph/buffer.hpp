#pragma once
#include "gl_handle.hpp"
#include <stdexcept>

namespace ag {

struct buffer_slice {
  GLuint obj;
  size_t offset;
  size_t size;
};

enum class buffer_usage {
  upload,        // CPU-visible, write-only
  default_usage, // GPU-visible, cannot be accessed by the CPU
  readback       // CPU-visible, read-only
};

struct buffer_deleter {
  void operator()(GLuint obj) { glDeleteBuffers(1, &obj); }
};

class buffer {
public:
  buffer() = default;
  buffer(size_t byteSize, buffer_usage usage, GLuint obj)
      : usage_{usage}, byte_size_{byteSize}, obj_{obj} {}

  GLuint object() const { return obj_.get(); }
  buffer_usage get_usage() const { return usage_; }
  size_t size() const { return byte_size_; }
  void *map(size_t offset, size_t size);
  buffer_slice as_slice() const {
    return buffer_slice{obj_.get(), 0, byte_size_};
  }

  static buffer create(size_t byteSize, buffer_usage usage,
                       const void *initial_data = nullptr);

private:
  buffer_usage usage_ = buffer_usage::default_usage;
  size_t byte_size_ = 0;
  gl_handle<buffer_deleter> obj_;
};
}