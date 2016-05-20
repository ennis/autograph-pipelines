#pragma once
#include "element.hpp"
#include "ruler.hpp"

struct GLFWwindow;

namespace ui {

class native_window : public container {
public:
  native_window(GLFWwindow *window) : window_{ window }
  {}

  glm::ivec2 measure(renderer &r) override {
    ruler a;
    for (auto p : children()) {
      if (p)
        a.append(p->cache_measure(r), ruler::placement::vertical);
    }
    return a.size();
  }

  void render(renderer &r, const rect_2d &geometry) override;

private:
  GLFWwindow *window_;
};
}