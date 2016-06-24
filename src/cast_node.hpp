#pragma once

#include "node.hpp"
#include "image.hpp"

struct cast_node : public node {
  cast_node() : node{node_kind::cast} {}

  static bool classof(const node &n) { return n.kind() == node_kind::cast; }

  image_impl src;
  image_impl dest;
};
