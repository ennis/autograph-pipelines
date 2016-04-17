#pragma once
#include <gl_core_4_5.hpp>
#include "format.hpp"
#include "node.hpp"
#include "value.hpp"
#include "image_desc.hpp"
#include "gl_texture.hpp"

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
	  uint8_t* linear_data;
	  gl_texture* device_tex;
  } storage;
};

/////////////////////////////////////////////////////
// Proxy for image_impl
class image {
public:
  static image clear_1d(image_format format, unsigned width,
                        const glm::vec4 &rgba);
  static image clear_2d(image_format format, unsigned width, unsigned height,
                        const glm::vec4 &rgba);
  static image clear_3d(image_format format, unsigned width, unsigned height,
                        unsigned depth, const glm::vec4 &rgba);

  image subimage(const rect_2d &rect);

  image cast(image_format format);

  image &set_storage_hint(storage_hint hint);

  image eager();

  template <typename... Resources>
  image filter(const char* glsl, Resources&&... resources)
  {
  }

  // mark this image for rescheduling
  image &schedule();

  auto &name() const { return impl_->name_; }

  image &set_name(std::string name) {
    impl_->name_ = std::move(name);
    return *this;
  }

  image(std::shared_ptr<image_impl> impl) : impl_(impl) {}

  std::shared_ptr<image_impl> impl_;
};