#pragma once
#include <autograph/support/Utils.h>

namespace ag {
enum class ShaderStage {
  Vertex = 1 << 0,
  Geometry = 1 << 1,
  Fragment = 1 << 2,
  TessControl = 1 << 3,
  TessEval = 1 << 4,
  Compute = 1 << 5
};
ENUM_BIT_FLAGS_OPERATORS(ShaderStage)
}
