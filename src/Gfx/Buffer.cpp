#include <autograph/Gfx/Buffer.h>
#include <stdexcept>

namespace ag {
void *Buffer::map(size_t offset, size_t size) {
  gl::GLbitfield flags = gl::MAP_UNSYNCHRONIZED_BIT;
  if (usage_ == Usage::Readback) {
    flags |= gl::MAP_READ_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else if (usage_ == Usage::Upload) {
    flags |= gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else {
    // cannot map a DEFAULT buffer
    throw std::logic_error(
        "Trying to map a buffer allocated with gl_buffer_usage::Default");
  }
  return gl::MapNamedBufferRange(object(), offset, size, flags);
}

Buffer::Buffer(std::size_t byteSize, Buffer::Usage usage,
               const void *initial_data)
    : usage_{usage}, byte_size_{byteSize} {
  gl::GLbitfield flags = 0;
  if (usage == Usage::Readback) {
    flags |= gl::MAP_READ_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else if (usage == Usage::Upload) {
    flags |= gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else {
    flags = 0;
  }

  gl::GLuint buf_obj;
  gl::CreateBuffers(1, &buf_obj);
  gl::NamedBufferStorage(buf_obj, byteSize, initial_data, flags);
  obj_ = buf_obj;
}
} // namespace ag