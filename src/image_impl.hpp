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
};

struct image_resource {
  image_resource() : u{nullptr} {}

  enum image_resource_type {
    texture,
    texture_region,
    texture_layer
  } type = texture;
  union U {
    struct {
      gl_texture *gl_tex;
    } tex;
    struct {
      gl_texture *gl_tex;
      int layer;
    } tex_layer;
    struct {
      gl_texture *gl_tex;
      rect_2d region;
    } tex_region;
  } u;
  // TODO: images in host memory
};

struct image_impl : public value_impl {
  using ptr = std::shared_ptr<image_impl>;

  // create an empty value
  image_impl() : value_impl{value_kind::image} {}

  // create a new value bound to an existing one
  image_impl(ptr share, node *pred)
      : value_impl{value_kind::image, std::move(share)} {}

  // create a new 'standalone' specified (typed) value
  image_impl(image_desc desc, node *pred)
      : value_impl{value_kind::image, value_flags::specified}, desc_{desc} {}

  static bool classof(const value_impl &v) {
    return v.kind() == value_kind::image;
  }

  gl_texture &get_texture() { 
	  assert(is_allocated());
	  return *res_.u.tex.gl_tex; 
  }

  // bind a value
  image_impl& bind(image_impl::ptr from)
  {

  }

  image_impl::ptr as_texture() {
    auto ptr = std::make_shared<image_impl>(desc_, nullptr);
    ptr->res_.type = image_resource::texture;
    // ptr->share_ = this;
    return std::move(ptr);
  }

  static image_impl::ptr create_2d(glm::ivec2 size, image_format format,
                                   node *pred, int num_mips = 1) {
    image_desc desc;
    desc.dimensions = image_dimensions::img_2d;
    desc.width = size.x;
    desc.height = size.y;
    desc.depth = 1;
    desc.format = format;
    desc.num_mips = 1;
    return std::make_shared<image_impl>(desc, pred);
  }

  // image description
  image_desc desc_;
  // actual resource
  image_resource res_;
  // texture object owned by the node, if there is one
  gl_texture gl_tex_;
};
