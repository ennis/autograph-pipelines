#pragma once
#include "element.hpp"
#include "renderer.hpp"
#include "sprite.hpp"
#include "button_behavior.hpp"

namespace ui {

class button : public button_behavior {
public:
  button(element &parent, std::string label);
  button(element &parent, std::string label, ui::sprite &icon);

  void set_icon(ui::sprite &icon);

  virtual glm::ivec2 measure(renderer &r) override;
  virtual void render(renderer &r) override;

private:
  std::string label_;
  ui::sprite *icon_{nullptr};
};
}