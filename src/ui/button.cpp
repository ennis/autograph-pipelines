#include "button.hpp"
#include "visual_state.hpp"

namespace ui {
button::button(std::string label)
    : label_{std::move(label)} {}


glm::ivec2 button::measure(renderer &r) {
  return r.measure_button(label_, icon_);
}

void button::render(renderer &r) {
  return r.render_button(geometry(), label_, icon_, visual_state::default_);
}
}