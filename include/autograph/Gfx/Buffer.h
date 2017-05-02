#pragma once
#include <autograph/Config.h>
#include "GLHandle.h"
#include <stdexcept>

namespace ag {

//////////////////////////////////////////////
struct BufferSlice {
  gl::GLuint obj;
  size_t offset;
  size_t size;
};

//////////////////////////////////////////////
enum class BufferUsage {
  Upload,  // CPU-visible, write-only
  Default, // GPU-visible, cannot be accessed by the CPU
  Readback // CPU-visible, read-only
};

//////////////////////////////////////////////
struct AG_GFX_API BufferDeleter {
	static constexpr gl::GLenum objectType = gl::BUFFER;
  void operator()(gl::GLuint obj) { gl::DeleteBuffers(1, &obj); }
};

class AG_GFX_API Buffer {
public:
  Buffer() = default;
  Buffer(size_t byteSize, BufferUsage usage, const void *initial_data = nullptr);

  gl::GLuint object() const { return obj_.get(); }
  BufferUsage getUsage() const { return usage_; }
  size_t size() const { return byte_size_; }
  void *map(size_t offset, size_t size);
  BufferSlice asSlice() const { return BufferSlice{obj_.get(), 0, byte_size_}; }

private:
  BufferUsage usage_ = BufferUsage::Default;
  size_t byte_size_ = 0;
  GLHandle<BufferDeleter> obj_;
};
}