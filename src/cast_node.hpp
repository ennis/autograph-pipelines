#pragma once

#include "node.hpp"
#include "image.hpp"

struct cast_node : public node {
  cast_node() : node{node_kind::cast} {}

  static bool classof(const node &n) { return n.kind() == node_kind::cast; }

  std::shared_ptr<image_impl> src;
  image_impl dest;

  static std::shared_ptr<image_impl> create(std::shared_ptr<image_impl> src, image_format fmt)
  {
  	auto n = std::make_shared<cast_node>();
  	n->src = std::move(src);
  	image_desc dest_desc = n->src->desc_;
  	dest_desc.format = fmt;
  	n->dest = image_impl{n.get(), dest_desc, 0};
  	return std::shared_ptr<image_impl>{n, &n->dest};
  }  
};
