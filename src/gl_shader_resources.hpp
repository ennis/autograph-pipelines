#pragma once
#include <vector>
#include <gl_core_4_5.hpp>
#include "utils.hpp"

enum class gl_shader_resource_state_mask {
  texture_units = (1 << 0),
  image_units = (1 << 1),
  shader_storage_buffers = (1 << 2),
  uniform_buffers = (1 << 3),
  vertex_buffers = (1 << 4),
  index_buffer = (1 << 5),
  sampler_objects = (1 << 6),
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
  GLuint ibo;
  GLenum index_type;

  void bind_texture(unsigned slot, GLuint obj)
  {
	  textures[slot] = obj;
  }

  void bind_image(unsigned slot, GLuint obj)
  {
	  images[slot] = obj;
  }

  void bind_sampler(unsigned slot, GLuint obj)
  {
	  samplers[slot] = obj;
  }

  void bind_vertex_buffer(unsigned slot, GLuint handle, size_t offset,
	  size_t size, unsigned stride) {
	  vbo[slot] = handle;
	  vbo_offsets[slot] = offset;
	  vbo_strides[slot] = stride;
  }

  void bind_index_buffer(GLuint handle, size_t offset, size_t size,
	  GLenum type) {
	  //i = type;
	  ibo = handle;
  }

  void bind_uniform_buffer(unsigned slot, GLuint handle, size_t offset,
	  size_t size) {
	  ubo[slot] = handle;
	  ubo_sizes[slot] = size;
	  ubo_offsets[slot] = offset;
  }
  
  void bind_storage_buffer(unsigned slot, GLuint handle, size_t offset,
	  size_t size) {
	  ssbo[slot] = handle;
	  ssbo_sizes[slot] = size;
	  ssbo_offsets[slot] = offset;
  }

  void reset();
  void bind();
};
