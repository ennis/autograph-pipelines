#pragma once
#include "gsl.h"
#include "node.hpp"
#include "pipeline_program.hpp"
#include "shader_resource.hpp"
#include "value.hpp"

namespace detail {
inline int div_round_up(int numToRound, int multiple) {
  return (numToRound + multiple - 1) / multiple;
}
}

struct compute_workspace {
  unsigned x;
  unsigned y;
  unsigned z;

  static compute_workspace make_2d(const extents_2d &global_size,
                                   const extents_2d &local_size) {
    return compute_workspace{
        (unsigned)detail::div_round_up(global_size.width, local_size.width),
        (unsigned)detail::div_round_up(global_size.height, local_size.height),
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
  compute_pipeline_program* pp;
  shader_resources res;

  virtual void traverse(traversal_visitor &v) override {
    for (auto &r : res) {
      if (not_empty(r.access & shader_resource_access::write)) {
        v.visit_value(*r.resource);
      }
    }
  }

  static std::shared_ptr<compute_node> create(compute_pipeline_program& prog,
                     const compute_workspace &ws, shader_resources res) 
  {
	  auto n = std::make_shared<compute_node>();
	  n->ws = ws;
	  n->res = std::move(res);
	  n->pp = &prog;
	  return std::move(n);
  }
};
