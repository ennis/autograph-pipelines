#include <autograph/AABB.h>

namespace ag {
AABB AABB::transform(const mat4 &t) {
  AABB out{100000.0f, 100000.0f, 100000.0f, -100000.0f, -100000.0f, -100000.0f};
  vec4 vs[8] = {
      t * vec4{xmin, ymin, zmin, 1.0f}, t * vec4{xmin, ymin, zmax, 1.0f},
      t * vec4{xmin, ymax, zmin, 1.0f}, t * vec4{xmin, ymax, zmax, 1.0f},
      t * vec4{xmax, ymin, zmin, 1.0f}, t * vec4{xmax, ymin, zmax, 1.0f},
      t * vec4{xmax, ymax, zmin, 1.0f}, t * vec4{xmax, ymax, zmax, 1.0f}};

  for (auto &&v : vs) {
    if (out.xmin > v.x)
      out.xmin = v.x;
    if (out.xmax < v.x)
      out.xmax = v.x;
    if (out.ymin > v.y)
      out.ymin = v.y;
    if (out.ymax < v.y)
      out.ymax = v.y;
    if (out.zmin > v.z)
      out.zmin = v.z;
    if (out.zmax < v.z)
      out.zmax = v.z;
  }

  return out;
}
}
