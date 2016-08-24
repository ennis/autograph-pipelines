#pragma once
#include "../entity.hpp"
#include <reflect.hpp>
#include <glm/glm.hpp>

struct NVGcontext;

namespace ui {
struct REFLECT visual : public behaviour<visual> {
  virtual ~visual() {}

  virtual void render(NVGcontext *nvg, glm::vec2 size) = 0;
};

struct REFLECT vector_visual : public visual {
  using callback_t = std::function<void(NVGcontext *, glm::vec2)>;

  vector_visual() = default;
  vector_visual(callback_t fn) : commands{std::move(fn)} {}

  void render(NVGcontext *nvg, glm::vec2 size) override {
    if (commands)
      commands(nvg, size);
  }

  callback_t commands;
};
}