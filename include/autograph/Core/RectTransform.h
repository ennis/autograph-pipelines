#pragma once
#include <autograph/Core/Types.h>
#include <autograph/Core/Rect.h>

namespace ag {
//////////////////////////////////////////////
// Rect transform
// 2D transform + depth-order
// reduces to a 2D transformation matrix (w/o scaling) + z-order + size + scale
struct AG_CORE_API RectTransform {
  // rect corners relative to anchors
  vec2 offsetA{0.0f, 0.0f};
  vec2 offsetB{0.0f, 0.0f};
  // upper-left anchor rect position (normalized)
  vec2 anchorA{0.0f, 0.0f};
  // lower-right anchor rect position (normalized)
  vec2 anchorB{1.0f, 1.0f};
  // pivot position relative to anchored rectangle
  vec2 pivot{0.5f, 0.5f};
  // rotation amount aroud pivot
  vec3 rotation{0.0f, 0.0f, 0.0f};
  // scale around pivot
  vec3 scale{1.0f, 1.0f, 1.0f};
  // z-order
  float zOrder{0.0f};

  struct Calculated {
    mat3 transform;
	mat3 inverseTransform;
    vec2 size;

	// World space -> local space
	vec2 worldToLocal(vec2 pos) const {
		vec3 tmp = inverseTransform * vec3{ pos.x, pos.y, 1.0f };
		return vec2{ tmp.x, tmp.y };
	}
	// World space -> local space
	vec2 worldToLocal(ivec2 ipos) const {
		vec3 tmp = inverseTransform * vec3{ (float)ipos.x, (float)ipos.y, 1.0f };
		return vec2{ tmp.x, tmp.y };
	}

	bool isPointInside(vec2 point);
  };

  Calculated calculateTransform(const mat3& parentTransform, vec2 parentSize);

};

}