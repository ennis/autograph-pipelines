#include "shader_resource.hpp"

const char *get_shader_resource_type_name(shader_resource_type t) {
  switch (t) {
  case shader_resource_type::index_buffer:
    return "index_buffer";
  case shader_resource_type::sampled_image:
    return "sampled_image";
  case shader_resource_type::storage_buffer:
    return "storage_buffer";
  case shader_resource_type::storage_image:
    return "storage_image";
  case shader_resource_type::uniform_buffer:
    return "uniform_buffer";
  case shader_resource_type::vertex_buffer:
    return "vertex_buffer";
  default:
    return "other";
  }
}

