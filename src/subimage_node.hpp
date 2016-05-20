#pragma once
#include "image.hpp"

/////////////////////////////////////////////////////
// subimage
struct subimage_2d_node : public node {
  subimage_2d_node() : node{node_kind::subimage_2d} {}

  static bool classof(const node &n) {
    return n.kind() == node_kind::subimage_2d;
  }

  virtual void traverse(traversal_visitor &v) override { v.visit_value(dest); }

  std::shared_ptr<image_impl> src;
  image_impl dest;
  rect_2d rect;

  static auto create(std::shared_ptr<image_impl> src, const rect_2d &rect) {
    auto n = std::make_shared<subimage_2d_node>();
    auto desc = src->desc_;
    desc.width = rect.size.x;
    desc.height = rect.size.y;
    n->rect = rect;
    n->dest = image_impl{n.get(), desc, 0};
    return std::shared_ptr<image_impl>{n, &n->dest};
  }
};
