#include <autograph/Core/AABB.h>

namespace ag {

  AABB AABB::transform(const mat4 &m) const
  {
	  auto xa = m[0] * xmin;
	  auto xb = m[0] * xmax;

	  auto ya = m[1] * ymin;
	  auto yb = m[1] * ymax;

	  auto za = m[2] * zmin;
	  auto zb = m[2] * zmax;

	  auto min = glm::min(xa, xb) + glm::min(ya, yb) + glm::min(za, zb) + m[3];
	  auto max = glm::max(xa, xb) + glm::max(ya, yb) + glm::max(za, zb) + m[3];

	  return AABB{ min.x, min.y, min.z, max.x, max.y, max.z };
  }


AABB& AABB::unionWith(const AABB& other)
{
	xmin = glm::min(xmin, other.xmin);
	xmax = glm::max(xmax, other.xmax);
	ymin = glm::min(ymin, other.ymin);
	ymax = glm::max(ymax, other.ymax);
	zmin = glm::min(zmin, other.zmin);
	zmax = glm::max(zmax, other.zmax);
	return *this;
}

}
