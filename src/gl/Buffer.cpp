#include <autograph/gl/Buffer.h>
#include <stdexcept>

namespace ag {
  namespace gl {
void *Buffer::map(size_t offset, size_t size) {
  GLbitfield flags = GL_MAP_UNSYNCHRONIZED_BIT;
  if (usage_ == BufferUsage::Readback) {
    flags |= GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else if (usage_ == BufferUsage::Upload) {
    flags |= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else {
    // cannot map a DEFAULT buffer
    throw std::logic_error(
        "Trying to map a buffer allocated with gl_buffer_usage::Default");
  }
  return glMapNamedBufferRange(object(), offset, size, flags);
}

Buffer::Buffer(std::size_t byteSize, BufferUsage usage,
	const void *initial_data) : usage_{ usage }, byte_size_{ byteSize } {
  GLbitfield flags = 0;
  if (usage == BufferUsage::Readback) {
    flags |= GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else if (usage == BufferUsage::Upload) {
    flags |= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else {
    flags = 0;
  }

  GLuint buf_obj;
  glCreateBuffers(1, &buf_obj);
  glNamedBufferStorage(buf_obj, byteSize, initial_data, flags);
  obj_ = buf_obj;
}
}}