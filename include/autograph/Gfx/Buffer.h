#pragma once
#include "GLHandle.h"
#include <autograph/Config.h>
#include <stdexcept>

namespace ag {

//////////////////////////////////////////////
struct AG_GFX_API BufferDeleter {
  static constexpr gl::GLenum objectType = gl::BUFFER;
  void operator()(gl::GLuint obj) { gl::DeleteBuffers(1, &obj); }
};

class AG_GFX_API Buffer {
public:
  //////////////////////////////////////////////
  struct Slice {
    gl::GLuint obj;
    size_t offset;
    size_t size;
  };

  //////////////////////////////////////////////
  enum class Usage {
    Upload,  // CPU-visible, write-only
    Default, // GPU-visible, cannot be accessed by the CPU
    Readback // CPU-visible, read-only
  };

  Buffer() = default;
  Buffer(size_t byteSize, Usage usage,
         const void *initial_data = nullptr);

  gl::GLuint object() const { return obj_.get(); }
  Usage usage() const { return usage_; }
  size_t size() const { return byte_size_; }
  void *map(size_t offset, size_t size);
  Slice asSlice() const { return Slice{obj_.get(), 0, byte_size_}; }

private:
  Usage usage_ = Usage::Default;
  size_t byte_size_ = 0;
  GLHandle<BufferDeleter> obj_;
};
} // namespace ag
