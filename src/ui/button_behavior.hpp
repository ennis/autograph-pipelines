#pragma once
#include "../observable.hpp"
#include "element.hpp"
#include "input.hpp"
#include "visual_state.hpp"

namespace ui {
class button_behavior : public element {
public:
  button_behavior(element *parent) : element{parent} {}

  observable<> hover_enter;
  observable<> hover_exit;
  observable<> pressed;
  observable<> released;
  observable<> drag_enter;
  observable<> drag_exit;

  void process_input(const input::input_event &ev,
                     scheduler &event_sched) override {
    auto geom = geometry();
    if (auto cursorEvent = ev.as<input::cursor_event>()) {
      bool was_hovering = hovering_;
      if (geom.inside(cursorEvent->pos)) {
        hovering_ = true;
        if (!was_hovering)
          hover_enter.signal_deferred(event_sched);
      } else {
        hovering_ = false;
        if (was_hovering)
          hover_exit.signal_deferred(event_sched);
      }
    } else if (auto mouseButtonEvent = ev.as<input::mouse_button_event>()) {
      if (hovering_) {
        // LMB
        if (mouseButtonEvent->button == 0) {
          if (!pressed_) {
            pressed_ = true;
            pressed.signal_deferred(event_sched);
          } else {
            pressed_ = true;
          }
        }
      }
      if (mouseButtonEvent->button == 0 &&
          mouseButtonEvent->state == input::button_state::released) {
        pressed_ = false;
      }
    }
  }

private:
  glm::ivec2 drag_origin_;
  glm::ivec2 drag_current_;
  bool hovering_{false};
  bool pressed_{false};
  visual_state state_;
};
}