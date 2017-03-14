#pragma once
#include <autograph/Config.h>
#include "GLHandle.h"
#include <stdexcept>

namespace ag {
namespace gl {

//////////////////////////////////////////////
struct BufferSlice {
  GLuint obj;
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
struct AG_GL_API BufferDeleter {
	static constexpr GLenum objectType = GL_BUFFER;
  void operator()(GLuint obj) { glDeleteBuffers(1, &obj); }
};

class AG_GL_API Buffer {
public:
  Buffer() = default;
  Buffer(size_t byteSize, BufferUsage usage, const void *initial_data = nullptr);

  GLuint object() const { return obj_.get(); }
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
}