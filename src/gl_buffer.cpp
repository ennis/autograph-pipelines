#include "gl_buffer.hpp"
#include <stdexcept>

/*void* map_buffer(GLuint handle, gl_buffer_usage usage, size_t offset,
        size_t size) {
        // all our operations are unsynchronized
        GLbitfield flags = gl::MAP_UNSYNCHRONIZED_BIT;
        if (usage == BufferUsage::Readback) {
                flags |= gl::MAP_READ_BIT | gl::MAP_PERSISTENT_BIT |
gl::MAP_COHERENT_BIT;
        }
        else if (usage == BufferUsage::Upload) {
                flags |= gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT |
gl::MAP_COHERENT_BIT;
        }
        else {
                // cannot map a DEFAULT buffer
                throw std::logic_error(
                        "Trying to map a buffer allocated with
BufferUsage::Default");
        }
        return gl::MapNamedBufferRange(handle, offset, size, flags);
}*/

gl_buffer gl_buffer::create(std::size_t byteSize, gl_buffer_usage usage,
                            const void *initial_data) {
  GLbitfield flags = 0;
  if (usage == gl_buffer_usage::readback) {
    flags |= gl::MAP_READ_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else if (usage == gl_buffer_usage::upload) {
    flags |= gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else {
    flags = 0;
  }

  GLuint buf_obj;
  gl::CreateBuffers(1, &buf_obj);
  gl::NamedBufferStorage(buf_obj, byteSize, initial_data, flags);
  return gl_buffer{byteSize, usage, buf_obj};
}
