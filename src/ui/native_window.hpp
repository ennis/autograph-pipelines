#pragma once
#include "element.hpp"
#include "ruler.hpp"

struct GLFWwindow;

namespace ui {

class native_window : public container {
public:
  native_window(GLFWwindow *window) : window_{window} {}

  glm::ivec2 measure(renderer &r) override {
    ruler a;
    for (auto p : children()) {
      if (p)
        a.append(p->cache_measure(r), ruler::placement::vertical);
    }
    return a.size();
  }

  void process_input(const input::input_event &ev,
                     scheduler &event_sched) override {
    for (auto p : children()) {
      if (p)
        // XXX should forward input to child only if it is visible
        p->process_input(ev, event_sched);
    }
  }

  void render(renderer &r) override;

  GLFWwindow* get_window_impl() {
	  return window_;
  }

private:
  GLFWwindow *window_;
};
}