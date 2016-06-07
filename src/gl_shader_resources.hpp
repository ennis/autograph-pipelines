#pragma once
#include <vector>
#include <gl_core_4_5.hpp>
#include "utils.hpp"
#include "gl_buffer.hpp"

enum class gl_shader_resource_state_mask {
  texture_units = (1 << 0),
  image_units = (1 << 1),
  shader_storage_buffers = (1 << 2),
  uniform_buffers = (1 << 3),
  vertex_buffers = (1 << 4),
  index_buffer = (1 << 5),
  sampler_objects = (1 << 6),
  draw_attachements = (1 << 7),
  all = 0xFF
};

ENUM_BIT_FLAGS_OPERATORS(gl_shader_resource_state_mask)

struct gl_shader_resources {
  gl_shader_resource_state_mask mask;
  std::vector<GLuint> vbo;
  std::vector<GLintptr> vbo_offsets;
  std::vector<GLsizei> vbo_strides;
  std::vector<GLuint> textures;
  std::vector<GLuint> samplers;
  std::vector<GLuint> images;
  std::vector<GLuint> ubo;
  std::vector<GLsizeiptr> ubo_sizes;
  std::vector<GLintptr> ubo_offsets;
  std::vector<GLuint> ssbo;
  std::vector<GLsizeiptr> ssbo_sizes;
  std::vector<GLintptr> ssbo_offsets;
  gl_buffer_slice ibo;
  GLenum index_type;
  GLuint fbo;

  void reset();
  void bind();
};
