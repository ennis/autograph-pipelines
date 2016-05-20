#pragma once

namespace ui {
// utility class to measure the minimum size of widget elements
class ruler {
public:
  enum class placement { vertical, horizontal, top };

  void append(glm::ivec2 elem_size, placement p = placement::vertical) {
    switch (p) {
    case placement::vertical:
      size_.x = std::max(elem_size.x, size_.x);
      size_.y += elem_size.y;
      break;
    case placement::horizontal:
      size_.x += elem_size.x;
      size_.y = std::max(elem_size.y, size_.y);
      break;
    case placement::top:
      size_.x = std::max(elem_size.x, size_.x);
      size_.y = std::max(elem_size.y, size_.y);
      break;
    }
  }

  void set_min_width(int w) { size_.x = std::max(w, size_.x); }

  void set_min_height(int h) { size_.y = std::max(h, size_.y); }

  glm::ivec2 size() const { return size_; }

private:
  glm::ivec2 size_{0.0f, 0.0f};
};
}