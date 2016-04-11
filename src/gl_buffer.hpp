#pragma once
#include "gl_handle.hpp"
#include <gl_core_4_5.hpp>

enum class gl_buffer_usage {
  upload,        // CPU-visible, write-only
  default_usage, // GPU-visible, cannot be accessed by the CPU
  readback       // CPU-visible, read-only
};

struct buffer_deleter {
  void operator()(GLuint obj) { gl::DeleteBuffers(1, &obj); }
};

struct gl_buffer {
  gl_buffer() = default;
  gl_buffer(std::size_t byteSize, gl_buffer_usage usage, GLuint obj)
      : usage_{usage}, byte_size_{byteSize}, obj_{obj} {}

  GLuint object() const { return obj_.get(); }

  gl_buffer_usage usage_;
  std::size_t byte_size_;
  gl_handle<buffer_deleter> obj_;

  static gl_buffer create(std::size_t byteSize, gl_buffer_usage usage,
                          const void *initial_data = nullptr);
};

struct gl_buffer_slice
{
	GLuint obj;
	size_t offset;
	size_t size;
};
