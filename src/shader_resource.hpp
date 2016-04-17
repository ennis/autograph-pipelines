#pragma once
#include <memory>
#include <vector>

#include "value.hpp"

struct shader_resource_context {
  unsigned slot = 0;
  bool has_index_buffer = false;
};

enum class shader_resource_type {
  sampled_image,
  storage_image,
  uniform_buffer,
  storage_buffer,
  vertex_buffer,
  index_buffer,
  color_attachement,
  depth_attachement
};

const char *get_shader_resource_type_name(shader_resource_type t);

enum class shader_resource_access {
  read = (1 << 0),
  write = (1 << 1)
};

ENUM_BIT_FLAGS_OPERATORS(shader_resource_access)

struct shader_resource {
  std::shared_ptr<value_impl> resource;
  shader_resource_type type;
  shader_resource_access access;
  unsigned slot;
};

using shader_resources = std::vector<shader_resource>;

/*
void bind_resource(shader_resources &shr, bind_resource_context &ctx,
                   const image &in) {
  // by default, images go to a sampled image slot
  shader_binding b;
  b.bind_type = shader_binding_type::sampled_image;
  b.resource = in.impl_;
  b.slot = ctx.slot++;
  shr.emplace_back(std::move(b));
}

void bind_resource(shader_resources &shr, bind_resource_context &ctx,
                   const buffer &in) {
  shader_binding b;
  b.bind_type = shader_binding_type::uniform_buffer;
  b.resource = in.impl_;
  b.slot = ctx.slot++;
  shr.emplace_back(std::move(b));
}
*/