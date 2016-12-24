#pragma once

#include <autograph/Types.h>

namespace ag 
{

struct AABB {
  float xmin;
  float ymin;
  float zmin;
  float xmax;
  float ymax;
  float zmax;

  constexpr float width() const { return xmax - xmin; }
  constexpr float height() const { return ymax - ymin; }
  constexpr float depth() const { return zmax - zmin; }

  AABB transform(const mat4 &t) const;
  AABB& unionWith(const AABB& other);
};

}