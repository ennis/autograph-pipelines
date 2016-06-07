#pragma once
#include "gl_texture.hpp"
#include "image.hpp"

struct image_node : public node {
  image_node() : node{node_kind::image}, storage{nullptr} {}
  ~image_node() {
    if (storage_type_ == storage_type::host) {
      storage.linear_data.~unique_ptr<uint8_t[]>();
    } else {
      storage.device_tex.~unique_ptr<gl_texture>();
    }
  }

  static bool classof(const node &n) { return n.kind() == node_kind::image; }

  /////////////////////////////////////////////////////////////////////
  // device version
  static std::shared_ptr<image_impl> create(const image_desc &desc,
                                            const void *data) {
    // TODO
    assert(desc.num_mips == 1);
    // TODO
    assert(desc.dimensions == image_dimensions::img_1d ||
           desc.dimensions == image_dimensions::img_2d);

    auto n = std::make_shared<image_node>();
    n->dest = image_impl{n.get(), desc, 0};
    n->dest.desc_.storage_hint_ = storage_hint::device;
    n->storage_type_ = storage_type::device;

    auto tex = std::make_unique<gl_texture>(desc);
    auto glfmt = get_gl_image_format_info(desc.format);
    switch (desc.dimensions) {
    case image_dimensions::img_1d:
      gl::TextureSubImage1D(tex->obj_.get(), 0, 0, desc.width,
                            glfmt.external_fmt, glfmt.type, data);
      break;
    case image_dimensions::img_2d:
      gl::TextureSubImage2D(tex->obj_.get(), 0, 0, 0, desc.width, desc.height,
                            glfmt.external_fmt, glfmt.type, data);
      break;
    case image_dimensions::img_3d:
      break;
    }
    n->storage.device_tex = std::move(tex);
	n->dest.storage.device_tex = n->storage.device_tex.get();
	n->dest.stype = n->storage_type_;
    return std::shared_ptr<image_impl>{n, &n->dest};
  }

  /////////////////////////////////////////////////////////////////////
  // host version
  static std::shared_ptr<image_impl>
  create_host(const image_desc &desc, std::unique_ptr<uint8_t[]> data) {
    // TODO
    assert(desc.num_mips == 1);
    // TODO
    assert(desc.dimensions == image_dimensions::img_1d ||
           desc.dimensions == image_dimensions::img_2d);

    auto n = std::make_shared<image_node>();
    n->dest = image_impl{n.get(), desc, 0};
    n->storage_type_ = storage_type::host;
    n->storage.linear_data = std::move(data);
	n->dest.storage.linear_data = n->storage.linear_data.get();
	n->dest.stype = n->storage_type_;
    return std::shared_ptr<image_impl>{n, &n->dest};
  }

  virtual void traverse(node_traversal_func fn) override { fn(dest); }

  image_impl dest;
  storage_type storage_type_;

  union U {
    std::unique_ptr<uint8_t[]> linear_data;
    std::unique_ptr<gl_texture> device_tex;
    ~U() {}
  } storage;
};
