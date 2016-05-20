#include "button.hpp"
#include "visual_state.hpp"

namespace ui {
button::button(element &parent, std::string label)
    : element{&parent}, label_{std::move(label)} {}

button::button(element &parent, std::string label, ui::sprite &icon)
    : element{&parent}, label_{std::move(label)}, icon_{&icon} {}

void button::set_icon(ui::sprite &icon) { icon_ = &icon; }

glm::ivec2 button::measure(renderer &r) {
  return r.measure_button(label_, icon_);
}

void button::render(renderer &r, const rect_2d &geometry) {
  return r.render_button(geometry, label_, icon_, visual_state::default_);
}
}