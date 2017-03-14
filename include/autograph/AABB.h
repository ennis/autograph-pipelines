#pragma once
#include <autograph/Config.h>
#include <autograph/Types.h>

namespace ag 
{

struct AG_CORE_API AABB {
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