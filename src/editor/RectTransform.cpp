#include <autograph/RectTransform.h>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace ag 
{
RectTransform::Calculated RectTransform::calculateTransform(const mat3 &parentTransform, vec2 parentSize) {
  Calculated c;
  // anchor positions (relative)
  float anchor_top = parentSize.y * anchorA.y;
  float anchor_bottom = parentSize.y * anchorB.y;
  float anchor_left = parentSize.x * anchorA.x;
  float anchor_right = parentSize.x * anchorB.x;

  // rect corners
  float rect_top = anchor_top + offsetA.y;
  float rect_bottom = anchor_bottom + offsetB.y; // it can be negative
  float rect_left = anchor_left + offsetA.x;
  float rect_right = anchor_right + offsetB.x;

  // ignore scale for now
  c.size.x = std::round(rect_right - rect_left);
  c.size.y = std::round(rect_bottom - rect_top);

  auto pos = vec2{std::round(rect_left) + 0.5f, std::round(rect_top) + 0.5f};
  vec2 piv_trans = pivot * c.size;
  c.transform = parentTransform;
  c.transform = glm::translate(c.transform, pos + piv_trans);
  c.transform = glm::rotate(c.transform, rotation.x);
  c.transform = glm::translate(c.transform, -piv_trans);
  c.inverseTransform = glm::inverse(c.transform);
  return c;
}

bool RectTransform::Calculated::isPointInside(vec2 point) {
  // world-to-local transform
  auto invp = inverseTransform * vec3{point, 1.0f};
  if (invp.x >= 0.0f && invp.x <= size.x && invp.y >= 0.0f &&
      invp.y <= size.y)
    return true;
  return false;
}

}