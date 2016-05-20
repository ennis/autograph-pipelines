#pragma once
#include "element.hpp"
#include "renderer.hpp"
#include "sprite.hpp"
#include "visual.hpp"

namespace ui {

class button : public element {
public:
  button(element &parent, std::string label);
  button(element &parent, std::string label, ui::sprite &icon);

  void set_icon(ui::sprite &icon);

  virtual glm::ivec2 measure(renderer &r) override;
  virtual void render(renderer &r, const rect_2d &geometry) override;

private:
  std::string label_;
  ui::sprite *icon_{nullptr};
};
}