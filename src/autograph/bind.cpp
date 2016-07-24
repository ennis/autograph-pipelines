#include "bind.hpp"

namespace ag {
void shader_resource_bind_state::bind() {
  for (unsigned i = 0; i < max_vertex_buffers; ++i)
    if (vertexBuffers[i])
      glBindVertexBuffer(i, vertexBuffers[i], vertexBufferOffsets[i],
                         vertexBufferStrides[i]);
    else
      glBindVertexBuffer(i, 0, 0, 0);
  glBindTextures(0, max_texture_units, textures.data());
  glBindSamplers(0, max_texture_units, samplers.data());
  glBindImageTextures(0, max_image_units, images.data());
  for (unsigned i = 0; i < max_uniform_buffers; ++i) {
    if (uniformBuffers[i])
      glBindBufferRange(GL_UNIFORM_BUFFER, i, uniformBuffers[i],
                        uniformBufferOffsets[i], uniformBufferSizes[i]);
    else
      glBindBufferBase(GL_UNIFORM_BUFFER, i, 0);
  }

  if (indexBuffer)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  reset();
}

void shader_resource_bind_state::reset() {
  indexBuffer = 0;
  vertexBuffers.fill(0);
  vertexBufferOffsets.fill(0);
  vertexBufferStrides.fill(0);
  images.fill(0);
  textures.fill(0);
  samplers.fill(0);
  shaderStorageBuffers.fill(0);
  uniformBuffers.fill(0);
  uniformBufferSizes.fill(0);
  uniformBufferOffsets.fill(0);
}
}