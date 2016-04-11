#pragma once

#include "node.hpp"
#include "shader_resource.hpp"
#include "pipeline_program.hpp"
#include "image.hpp"

struct dynamic_draw_state {
  // fixed-function pipeline config
};

// also: type-generic pipelines/programs
// eg. if image type is RGBA8, use pipeline with FMT_RGBA8

struct draw_command_params {
  bool indexed;
  // TODO
};


struct draw_attachements {
  std::vector<std::shared_ptr<image_impl>> color;
  std::shared_ptr<image_impl> depth;
  // cache: fbo
};


// draw node
struct draw_node : public node {
  draw_node() : node{node_kind::draw} {}

  /*virtual std::vector<node::dependency> dependencies() const override {
    std::vector<node::dependency> deps;
    deps.reserve(res.size() + att.color.size() + (att.depth ? 1 : 0));
    for (auto &&r : res) {
      deps.push_back(node::dependency{"resource", r.resource.get()});
    }
    for (auto &&a : att.color) {
      deps.push_back(node::dependency{"attachement", a.get()});
    }
    if (att.depth)
      deps.push_back(node::dependency{"depth_attachement", att.depth.get()});
  }*/

  static bool classof(const node &n) { return n.kind() == node_kind::draw; }

  dynamic_draw_state ds;
  draw_command_params cmd;
  shader_resources res;
  draw_attachements att;
  graphics_pipeline_program pp;
};
