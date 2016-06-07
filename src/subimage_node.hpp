#pragma once
#include "image.hpp"

/////////////////////////////////////////////////////
// subimage
struct subimage_2d_node : public node {
  subimage_2d_node(std::shared_ptr<image_impl> src, const rect_2d &rect) :
	  node{node_kind::subimage_2d}, 
	  src_{std::move(src)},
	  rect_{ rect }
  {
	  auto desc = src->desc_;
	  desc.width = rect.size.x;
	  desc.height = rect.size.y;
	  dest_ = image_impl{ this, desc, 0 };
	  add_dependency(*src);
  }

  static bool classof(const node &n) {
    return n.kind() == node_kind::subimage_2d;
  }

  virtual void traverse(node_traversal_func fn) override { fn(dest_); }

  static auto create(std::shared_ptr<image_impl> src, const rect_2d &rect) {
    auto n = std::make_shared<subimage_2d_node>(std::move(src), rect);
    return std::shared_ptr<image_impl>{n, &n->dest_};
  }

  std::shared_ptr<image_impl> src_;
  image_impl dest_;
  rect_2d rect_;
};
