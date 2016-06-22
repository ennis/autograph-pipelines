#pragma once
#include <vector>
#include "node.hpp"
#include "image.hpp"

struct clear_node : public node {
  clear_node(glm::ivec2 size, image_format fmt, glm::vec4 clear_color) : 
	  node{node_kind::clear}, 
	  clear_color_{clear_color}
  {
	  output_ = image_impl::create_2d(size, fmt, this);
  }

  static bool classof(const node &n) { return n.kind() == node_kind::clear; }

  void validate_inputs(graph_context& ctx) override {
	  // nothing to do
  }

  void finalize(graph_context& ctx) override {
	  // same
  }

  void execute(graph_context& ctx) override {
	  auto& tex = output_->get_texture();
	  gl::ClearTexImage(tex.object(), 0, gl::RGBA, gl::FLOAT, &clear_color_[0]);
  }

  glm::vec4 clear_color_;

  image_impl::ptr output_;
};
