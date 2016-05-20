#pragma once

namespace input {
enum class mouse_button_state { pressed, released };
enum class key_state { pressed, released, repeat };

enum class input_event_type {
  mouse_button,
  mouse_move,
  cursor,
  mouse_scroll,
  key,
  stylus_proximity,
  stylus_properties
};

struct input_event {
  using ptr = std::shared_ptr<input_event>;
  input_event(input_event_type ty) : type{ty} {}

  input_event_type type;
};

template <input_event_type Type> struct t_input_event : public input_event {
  static constexpr input_event_type event_type = Type;
  t_input_event() : input_event{Type} {}
};

struct mouse_button_event
    : public t_input_event<input_event_type::mouse_button> {
  using ptr = std::shared_ptr<mouse_button_event>;
  unsigned button;
  mouse_button_state state;
};

struct cursor_event : public t_input_event<input_event_type::cursor> {
  using ptr = std::shared_ptr<cursor_event>;
  // in client units (pixels)
  unsigned positionX;
  unsigned positionY;
};

struct mouse_move_event : public t_input_event<input_event_type::mouse_move> {
  using ptr = std::shared_ptr<mouse_move_event>;
  double dx; // mysterious device units
  double dy;
};

struct mouse_scroll_event
    : public t_input_event<input_event_type::mouse_scroll> {
  using ptr = std::shared_ptr<mouse_scroll_event>;
  double dx;
  double dy;
};

struct key_event : public t_input_event<input_event_type::key> {
  using ptr = std::shared_ptr<key_event>;
  uint32_t code;
  key_state state;
};

struct stylus_proximity_event
    : public t_input_event<input_event_type::stylus_proximity> {
  using ptr = std::shared_ptr<stylus_proximity_event>;
  // TODO
  // Touch, Hover, Leave
};

struct stylus_properties_event
    : public t_input_event<input_event_type::stylus_properties> {
  using ptr = std::shared_ptr<stylus_properties_event>;
  // state (touch, hover)
  double x;
  double y;
  double pressure;
  double tilt;
};
}