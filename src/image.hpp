#pragma once
#include <gl_core_4_5.hpp>
#include "format.hpp"
#include "node.hpp"
#include "value.hpp"

enum class image_dimensions {
  img_1d,
  img_2d,
  img_3d,
  img_cube,
  img_1d_array,
  img_2d_array,
  img_cube_array
};

struct position_2d {
  unsigned x = 0;
  unsigned y = 0;
};

struct extents_2d {
  extents_2d() = default;
  extents_2d(int w_, int h_) : width{w_}, height{h_} {}

  // -1 means whole image
  int width = -1;
  int height = -1;
};

struct rect_2d {
  position_2d pos;
  extents_2d size;
};

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

struct image_desc {
  image_dimensions dimensions;
  image_format format;
  unsigned width;  // -1 if pipeline-dynamic
  unsigned height; // -1 if pipeline-dynamic
  unsigned depth;  // or array size
  unsigned num_mips;
  storage_hint storage_hint;
};

struct image_impl : public value_impl {
  image_impl() : value_impl{value_kind::image} {}

  image_impl(node *predecessor, const image_desc &desc, size_t port_id = 0)
      : value_impl{value_kind::image, predecessor, port_id}, desc_{desc} {}

  static bool classof(const value_impl &v) {
    return v.kind() == value_kind::image;
  }

  image_desc desc_;
};

struct image_node : public node {
  enum class storage_type { device, host };

  image_node() : node{node_kind::image}, storage{nullptr} {}
  ~image_node() {
    if (storage_type == storage_type::host) {
      storage.linear_data.~unique_ptr<uint8_t[]>();
    }
  }

  static bool classof(const node &n) { return n.kind() == node_kind::image; }

  static std::shared_ptr<image_impl> create(const image_desc &desc,
                                            std::unique_ptr<uint8_t[]> data) {
    // TODO
    assert(desc.num_mips == 1);
    // TODO
    assert(desc.dimensions == image_dimensions::img_1d ||
           desc.dimensions == image_dimensions::img_2d);

    auto n = std::make_unique<image_node>();
    n->dest = image_impl{n.get(), desc, 0};
    n->storage_type = (desc.storage_hint == storage_hint::device)
                          ? storage_type::device
                          : storage_type::host;
    if (n->storage_type == storage_type::device) {
      // TODO
      assert(false);
    } else {
      // move-construct
      n->storage.linear_data = std::move(data);
    }
  }

  image_impl dest;
  storage_type storage_type;

  union U {
    std::unique_ptr<uint8_t[]> linear_data;
    GLuint device_tex;
    ~U() {}
  } storage;
};

struct subimage_2d_node : public node {
  subimage_2d_node() : node{node_kind::subimage_2d} {}

  static bool classof(const node &n) {
    return n.kind() == node_kind::subimage_2d;
  }

  std::shared_ptr<image_impl> src;
  image_impl dest;
  rect_2d rect;

  static auto create(std::shared_ptr<image_impl> src, const rect_2d &rect) {
    auto n = std::make_shared<subimage_2d_node>();
    auto desc = src->desc_;
    desc.width = rect.size.width;
    desc.height = rect.size.height;
    n->rect = rect;
    n->dest = image_impl{n.get(), desc, 0};
    return std::shared_ptr<image_impl>{n, &n->dest};
  }
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

  template <typename Resources>
  image filter(const char* glsl, Resources...&& resources)
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