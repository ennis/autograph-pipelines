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

  rect_2d calc_rect(rect_2d parent) const {
    // anchor positions (relative)
    float anchor_top = parent.size.y * anchor_a.y;
    float anchor_bottom = parent.size.y * anchor_b.y;
    float anchor_left = parent.size.x * anchor_a.x;
    float anchor_right = parent.size.x * anchor_b.x;

    // rect corners
    float rect_top = anchor_top + offset_a.y;
    float rect_bottom = anchor_bottom + offset_b.y; // it can be negative
    float rect_left = anchor_left + offset_a.x;
    float rect_right = anchor_right + offset_b.x;

    cached_rect_.pos.x = parent.pos.x + std::round(rect_left) + 0.5f;
    cached_rect_.pos.y = parent.pos.y + std::round(rect_top) + 0.5f;
    cached_rect_.size.x = std::round(rect_right - rect_left);
    cached_rect_.size.y = std::round(rect_bottom - rect_top);
    return cached_rect_;
  }

  // cached calculated rect before scaling & rotation
  mutable rect_2d cached_rect_;
  mutable uint64_t last_update_;

  // debugging
  void debug(NVGcontext *vg);
  void test_gui();
};