#pragma once
#include "element.hpp"

namespace ui {
class slider : public element {
public:
  slider(element &parent, float min, float max)
      : element{&parent}, min_{min}, max_{max} {}

  glm::ivec2 measure(renderer &r) override { return r.measure_slider(); }

  void render(renderer &r, const rect_2d &geometry) override {
    return r.render_slider_float(geometry, 0.0f, 1.0f, 0.5f);
  }

private:
  float min_;
  float max_;
};
}