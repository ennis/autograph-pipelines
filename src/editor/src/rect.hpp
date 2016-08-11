#pragma once
#include <glm/glm.hpp>
#include <reflect.hpp>

struct REFLECT rect_2d {
  glm::vec2 pos;
  glm::vec2 size;
  glm::vec2 top_left() const { return pos; }
  glm::vec2 top_right() const { return pos + glm::vec2{size.x, 0.0f}; }
  glm::vec2 bottom_left() const { return pos + glm::vec2{0.0f, size.y}; }
  glm::vec2 bottom_right() const { return pos + glm::vec2{size.x, size.y}; }

  constexpr bool inside(glm::vec2 p) const {
    return (p.x <= pos.x + size.x) && (p.x >= pos.x) &&
           (p.y <= pos.y + size.y) && (p.y >= pos.y);
  }
};