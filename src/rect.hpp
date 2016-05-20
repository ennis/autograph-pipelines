#pragma once
#include <glm/glm.hpp>

struct position_2d {
  position_2d() = default;
  position_2d(unsigned x_, unsigned y_) : x{x_}, y{y_} {}
  unsigned x = 0;
  unsigned y = 0;
};

struct extents_2d {
  extents_2d() = default;
  extents_2d(int w_, int h_) : width{w_}, height{h_} {}

  // -1 means whole image
  int width = -1;
  int height = -1;
};

struct rect_2d {
  rect_2d() = default;

  rect_2d(glm::ivec2 pos_, glm::ivec2 size_ = glm::ivec2{-1, -1})
      : pos{pos_}, size{size_} {}

  rect_2d(unsigned x, unsigned y, int w, int h) : pos{x, y}, size{w, h} {}

  bool inside(glm::ivec2 p) const {
    return (p.x < (pos.x + size.x)) && (p.x >= pos.x) &&
           (p.y < (pos.y + size.y)) && (p.y >= pos.y);
  }

  glm::ivec2 pos;
  glm::ivec2 size;
};
