#pragma once
#include "node.hpp"
#include "shader_resource.hpp"
#include "value.hpp"
#include "pipeline_program.hpp"
#include "gsl.h"

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
        (unsigned)detail::div_round_up(global_size.height, local_size.height), 1};
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

  struct out_resource {
    unsigned orig_slot;
    std::unique_ptr<value_impl> value;
  };

  compute_workspace ws;
  std::shared_ptr<compute_pipeline_program> pp;
  shader_resources res;
  std::vector<out_resource> outres;

  static void create(const compute_pipeline_program &prog,
                     const compute_workspace &ws, const shader_resources &res,
                     gsl::span<std::shared_ptr<value_impl>> out_res) 
  {
    // fill out entries of out_res with pointers to the written-to nodes
  }
};
