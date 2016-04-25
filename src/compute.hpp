#ifndef COMPUTE_HPP
#define COMPUTE_HPP

#include "bind.hpp"

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

  static compute_workspace make_2d(unsigned gx, unsigned gy, unsigned lx, unsigned ly) {
    return compute_workspace{
        (unsigned)detail::div_round_up(gx, lx),
        (unsigned)detail::div_round_up(gy, ly),
        1};
  }

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

template <typename... ShaderResources>
void compute(gl_compute_pipeline &pp, const compute_workspace &ws,
             ShaderResources &&... resources) {
  BindContext context;
  bind_shader_resources_rec(context, resources...);
  pp.bind();
  g_device->bind_state();
  gl::DispatchCompute(ws.x, ws.y, ws.z);
  gl::MemoryBarrier(gl::TEXTURE_UPDATE_BARRIER_BIT); // congratulations, you
                                                     // found a random hack
}

#endif // !COMPUTE_HPP
