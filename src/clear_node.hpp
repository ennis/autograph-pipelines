#pragma once
#include <vector>
#include "node.hpp"
#include "image.hpp"

struct clear_node : public node {
  clear_node(glm::vec4 clear_color) : node{node_kind::clear}, clear_color_{clear_color}
  {}

  static bool classof(const node &n) { return n.kind() == node_kind::clear; }

  virtual void traverse(node_traversal_func fn) override {
	  fn(dest);
  }

  static std::shared_ptr<image_impl> create(const image_desc& desc, const glm::vec4& clear_color)
  {
  	auto n = std::make_shared<clear_node>(clear_color);
  	n->dest = image_impl{n.get(), desc, 0};
	return std::shared_ptr<image_impl>{n, &n->dest};
  }

  glm::vec4 clear_color_;

  // port 0
  image_impl dest;
};
