#pragma once

#include "image.hpp"
#include "node.hpp"
#include "gl_pipeline.hpp"
#include "shader_resource.hpp"

// also: type-generic pipelines/programs
// eg. if image type is RGBA8, use pipeline with FMT_RGBA8

struct draw_command_params {
  bool indexed;
  // TODO
  union {
    struct draw_ {
      uint32_t vertex_count;
      uint32_t instance_count;
      uint32_t first_vertex;
      uint32_t first_instance;
    } draw;
    struct draw_indexed_ {
      uint32_t index_count;
      uint32_t instance_count;
      uint32_t first_index;
      int32_t vertex_offset;
      uint32_t first_instance;
    } draw_indexed;
  };
};

struct draw_attachements {
  std::vector<std::shared_ptr<image_impl>> color;
  std::shared_ptr<image_impl> depth;
  // cached framebuffer object
  mutable GLuint fbo;
};

// draw node
// cannot allocate resources
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

  virtual void traverse(traversal_visitor &v) override {
    for (auto &a : att.color) {
      v.visit_value(*a);
    }

    if (att.depth) {
      v.visit_value(*att.depth);
    }

    for (auto &r : res) {
      if (not_empty(r.access & shader_resource_access::write)) {
        v.visit_value(*r.resource);
      }
    }
  }

  // dynamic draw state (overrides states set in pp)
  gl_draw_state ds;
  draw_command_params cmd;
  shader_resources res;
  draw_attachements att;
  gl_graphics_pipeline* pp;
};

// immediate draw
// create framebuffer for target attachements (get it from a cache?)
// accumulate all resources into shader_resources
// convert shader_resources into gl_shader_resources
//
// Ideally:
// store directly bindings into gl_shader_resources