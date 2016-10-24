#pragma once
#include "Bind.hpp"

namespace ag {
namespace gl {
namespace detail {
inline int div_round_up(int numToRound, int multiple) {
  return (numToRound + multiple - 1) / multiple;
}
}

struct ComputeWorkspace {
  unsigned x;
  unsigned y;
  unsigned z;

  static ComputeWorkspace make2D(unsigned gx, unsigned gy, unsigned lx,
                                 unsigned ly) {
    return ComputeWorkspace{(unsigned)detail::div_round_up(gx, lx),
                            (unsigned)detail::div_round_up(gy, ly), 1};
  }
};
}
}
