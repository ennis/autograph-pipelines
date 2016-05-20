#pragma once
#include "format.hpp"
#include "gl_texture.hpp"
#include "image_desc.hpp"
#include "node.hpp"
#include "value.hpp"
#include <gl_core_4_5.hpp>

// linear image data
struct image_view {
  // w, h, d, pitch, format, pointer
  image_format format;
  unsigned width;
  unsigned height;
  unsigned depth;
  size_t stride; // row stride
  void *data;

  // image_view sub_view()
};

/////////////////////////////////////////////////////
// image_impl
// TODO: manually assign resources (placement new for nodes & images)
struct image_impl : public value_impl {
  image_impl() : value_impl{value_kind::image} {}

  image_impl(node *predecessor, const image_desc &desc, size_t port_id = 0)
      : value_impl{value_kind::image, predecessor, port_id}, desc_{desc} {}

  static bool classof(const value_impl &v) {
    return v.kind() == value_kind::image;
  }

  image_desc desc_;

  ///////////////////////////////////////////
  storage_type stype;
  union U {
    uint8_t *linear_data;
    gl_texture *device_tex;
  } storage;
};