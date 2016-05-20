#pragma once
#include "gsl.h"
#include "node.hpp"
#include "gl_pipeline.hpp"
#include "shader_resource.hpp"
#include "value.hpp"

namespace detail {
inline int div_round_up(int numToRound, int multiple) {
  return (numToRound + multiple - 1) / multiple;
}
}

struct gl_texture;
struct gl_buffer;

struct compute_workspace {
  unsigned x;
  unsigned y;
  unsigned z;

  static compute_workspace make_2d(const glm::ivec2 &global_size,
                                   const glm::ivec2 &local_size) {
    return compute_workspace{
        (unsigned)detail::div_round_up(global_size.x, local_size.x),
        (unsigned)detail::div_round_up(global_size.y, local_size.y),
        1};
  }

  // make_2d(global size, work group size)
  // make_3d(global size, work group size)
};

// compute node
struct compute_node : public node {
  compute_node() : node{node_kind::compute_shader} {}

  static bool classof(const node &n) {
    return n.kind() == node_kind::compute_shader;
  }

  compute_workspace ws;
  gl_compute_pipeline* pp;
  shader_resources res;

  virtual void traverse(traversal_visitor &v) override;

  virtual void allocate_resources(allocation_context&) override;

  static std::shared_ptr<compute_node> create(gl_compute_pipeline& prog,
                     const compute_workspace &ws, shader_resources res) 
  {
	  auto n = std::make_shared<compute_node>();
	  n->ws = ws;
	  n->res = std::move(res);
	  n->pp = &prog;
	  return std::move(n);
  }

  ////////////////////////////////////////////////
  // allocated resources
  bool alloc_ = false;  
  std::vector<std::unique_ptr<gl_texture> > texres_;
  std::vector<std::unique_ptr<gl_buffer> > bufres_; 
};
