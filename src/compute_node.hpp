#pragma once
#include "node.hpp"
#include "gl_pipeline.hpp"
#include "shader_resource.hpp"
#include "image_impl.hpp"
#include "buffer.hpp"

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
};

// compute node
struct compute_node : public node {
  compute_node() : node{node_kind::compute_shader} {}

  static bool classof(const node &n) {
    return n.kind() == node_kind::compute_shader;
  }

  void bind_sampled_image(unsigned slot, image_impl::ptr img)
  {
  }

  // bind a readonly storage image
  void bind_storage_image(unsigned slot, image_impl::ptr img, shader_resource_access access)
  {
  }

  // bind a read/write storage image
  void bind_storage_image(unsigned slot, image_impl::ptr img_in, image_impl::ptr& img_out, shader_resource_access access)
  {
  }

  void bind_uniform_buffer(unsigned slot, buffer_impl::ptr buf)
  {
  }

  /*void bind_storage_buffer(buffer_impl::ptr buf, shader_resource_access access)
  {
  }*/
  
  compute_workspace ws;
  gl_compute_pipeline* pp;
  shader_resources res_;

  // input resources
  std::vector<image_impl::ptr> input_images_;
  std::vector<buffer_impl::ptr> input_buffers_;
  // output resources
  std::vector<image_impl::ptr> output_images_;
  std::vector<buffer_impl::ptr> output_buffers_;
};

