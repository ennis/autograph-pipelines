#pragma once
#include "image.hpp"

/////////////////////////////////////////////////////
// subimage
struct subimage_2d_node : public node {
  static bool classof(const node &n) {
    return n.kind() == node_kind::subimage_2d;
  }

  //virtual void traverse(node_traversal_func fn) override { fn(dest_); }

  std::shared_ptr<image_impl> src_;
  image_impl dest_;
  rect_2d rect_;
};
