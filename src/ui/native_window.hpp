#pragma once
#include "element.hpp"
#include "ruler.hpp"

struct GLFWwindow;

namespace ui {

class native_window : public container {
public:
  native_window(GLFWwindow *window) : owns_window_{false}, window_{window} {}
  native_window(const glm::ivec2 &initial_size, const std::string &title);
  ~native_window();

  observable<> should_close;

  glm::ivec2 measure(renderer &r) override {
    ruler a;
    for (auto p : children()) {
      if (p)
        a.append(p->cache_measure(r), ruler::placement::vertical);
    }
    return a.size();
  }

  void process_input(const input::input_event &ev,
                     scheduler &event_sched) override;
  void render(renderer &r) override;

  GLFWwindow *get_window_impl() { return window_; }

private:
  bool owns_window_;
  GLFWwindow *window_;
};
}