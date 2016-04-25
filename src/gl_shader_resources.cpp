#include "gl_shader_resources.hpp"

void gl_shader_resources::reset() {
  ibo = 0;
  // ibo_type = gl::UNSIGNED_INT;
  std::fill(vbo.begin(), vbo.end(), 0);
  std::fill(vbo_offsets.begin(), vbo_offsets.end(), 0);
  std::fill(vbo_strides.begin(), vbo_strides.end(), 0);
  std::fill(textures.begin(), textures.end(), 0);
  std::fill(samplers.begin(), samplers.end(), 0);
  std::fill(images.begin(), images.end(), 0);
  std::fill(ubo.begin(), ubo.end(), 0);
  std::fill(ubo_sizes.begin(), ubo_sizes.end(), 0);
  std::fill(ubo_offsets.begin(), ubo_offsets.end(), 0);
  std::fill(ssbo.begin(), ssbo.end(), 0);
  std::fill(ssbo_sizes.begin(), ssbo_sizes.end(), 0);
  std::fill(ssbo_offsets.begin(), ssbo_offsets.end(), 0);
}

void gl_shader_resources::bind() {
  if (not_empty(mask & gl_shader_resource_state_mask::vertex_buffers)) {
    gl::BindVertexBuffers(0, (GLsizei)vbo.size(), vbo.data(),
                          vbo_offsets.data(), vbo_strides.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::uniform_buffers)) {
    gl::BindBuffersRange(gl::UNIFORM_BUFFER, 0, (GLsizei)ubo.size(),
                         ubo.data(), ubo_offsets.data(),
                         ubo_sizes.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::texture_units)) {
    gl::BindTextures(0, (GLsizei)textures.size(), textures.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::image_units)) {
    gl::BindImageTextures(0, (GLsizei)images.size(), images.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::shader_storage_buffers)) {
    gl::BindBuffersRange(gl::SHADER_STORAGE_BUFFER, 0, (GLsizei)ssbo.size(),
                         ssbo.data(), ssbo_offsets.data(),
                         ssbo_sizes.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::sampler_objects)) {
    gl::BindSamplers(0, (GLsizei)samplers.size(), samplers.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::index_buffer)) {
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
  }
}
