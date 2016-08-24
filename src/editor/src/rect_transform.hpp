#pragma once
#include <glm/glm.hpp>
#include <reflect.hpp>
#include <vector>

#include "entity.hpp"
#include "rect.hpp"

struct NVGcontext;

struct Range
{
	float min, max;
};

// Rect transform component
// 2D transform + depth-order
// reduces to a 2D transformation matrix (w/o scaling) + z-order + size + scale
struct REFLECT rect_transform : public component<rect_transform>
{
  rect_transform(entity::ptr parent_ = nullptr) : parent{parent_} {
  }

  // parent entity or nullptr
  entity::ptr parent META_FRIENDLY_NAME("Parent entity");
  // child entities
  std::vector<entity::ptr> children;
  // rect corners relative to anchors
  glm::vec2 offset_a META_FRIENDLY_NAME("Offset A") {0.0f, 0.0f};
  glm::vec2 offset_b META_FRIENDLY_NAME("Offset B") {0.0f, 0.0f};
  // upper-left anchor rect position (normalized)
  glm::vec2 anchor_a META_FRIENDLY_NAME("Anchor A") META(Range{ 0.0f, 1.0f }) {0.0f, 0.0f};
  // lower-right anchor rect position (normalized)
  glm::vec2 anchor_b META_FRIENDLY_NAME("Anchor B") META(Range{ 0.0f, 1.0f }) {1.0f, 1.0f};
  // pivot position relative to anchored rectangle
  glm::vec2 pivot META_FRIENDLY_NAME("Pivot") {0.5f, 0.5f};
  // rotation amount aroud pivot
  glm::vec3 rotation META_FRIENDLY_NAME("Rotation") {0.0f, 0.0f, 0.0f};
  // scale around pivot
  glm::vec3 scale META_FRIENDLY_NAME("Scale") {1.0f, 1.0f, 1.0f};
  // z-order
  float z_order META_FRIENDLY_NAME("Z-order") { 0.0f };

  // unconditionnally recomputes the calculated transformation matrix and size (ignores dirty flag)
  // relative to a parent transform 
  void update_transform(const glm::mat3& parent_tr, glm::vec2 parent_size);
  // hit-test
  bool point_inside(glm::vec2 world_pos)
  {
	  // world-to-local transform
	  auto w2l = glm::inverse(calc_transform);
	  auto invp = w2l * glm::vec3{ world_pos, 1.0f };
	  if (invp.x >= 0.0f && invp.x <= calc_size.x &&  invp.y >= 0.0f && invp.y <= calc_size.y)
		  return true;
	  return false;
  }

  // calculated local-to-world transform (minus scale)
  glm::mat3 calc_transform;
  glm::vec2 calc_size;
  // flag to signal that a recomputation is necessary
  bool dirty = false;

  // cached calculated rect before scaling & rotation
  //mutable glm::mat3 transform_;
  //mutable uint64_t last_update_;

  // debugging
  void debug(NVGcontext *vg);
  void test_gui();
};