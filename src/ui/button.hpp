#pragma once
#include "element.hpp"
#include "renderer.hpp"
#include "sprite.hpp"
#include "button_behavior.hpp"

namespace ui {

class button : public button_behavior {
public:
  button(std::string label);

  virtual glm::ivec2 measure(renderer &r) override;
  virtual void render(renderer &r) override;

private:
  std::string label_;
  ui::sprite *icon_{nullptr};
};
}