#pragma once

namespace ui {
class behavior {
public:
	behavior(visual& visual_)
	{}

  enum class state {
    pressed,
    hovered,
    none
  };

  state state() const { return state_; }

  observable<> hover_enter;
  observable<> hover_exit;
  observable<> click;
  observable<> click_break;
  observable<> drag_enter;
  observable<> drag_exit;

  void update(visual& visual, input_event& event);

private:
  state state_;
  bool prev_hit_;
};

}