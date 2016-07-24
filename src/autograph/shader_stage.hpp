#pragma once
#include "utils.hpp"

namespace ag 
{
enum class shader_stage {
  vertex = 1 << 0,
  geometry = 1 << 1,
  fragment = 1 << 2,
  tess_control = 1 << 3,
  tess_eval = 1 << 4,
  compute = 1 << 5
};
ENUM_BIT_FLAGS_OPERATORS(shader_stage)
}