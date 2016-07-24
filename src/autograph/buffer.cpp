#include "buffer.hpp"
#include <stdexcept>

namespace ag {
void *buffer::map(size_t offset, size_t size) {
  GLbitfield flags = GL_MAP_UNSYNCHRONIZED_BIT;
  if (usage_ == buffer_usage::readback) {
    flags |= GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else if (usage_ == buffer_usage::upload) {
    flags |= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else {
    // cannot map a DEFAULT buffer
    throw std::logic_error(
        "Trying to map a buffer allocated with gl_buffer_usage::default_usage");
  }
  return glMapNamedBufferRange(object(), offset, size, flags);
}

buffer buffer::create(std::size_t byteSize, buffer_usage usage,
                      const void *initial_data) {
  GLbitfield flags = 0;
  if (usage == buffer_usage::readback) {
    flags |= GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else if (usage == buffer_usage::upload) {
    flags |= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  } else {
    flags = 0;
  }

  GLuint buf_obj;
  glCreateBuffers(1, &buf_obj);
  glNamedBufferStorage(buf_obj, byteSize, initial_data, flags);
  return buffer{byteSize, usage, buf_obj};
}
}