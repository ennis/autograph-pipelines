#pragma once

// input system:
// RAW: raw inputs from GLFW, XInput
// TRANSLATED: commands
//
// polling: stylus position, gamepad, cursor position
// event-based: keys, mouse move, mouse button

namespace input {
enum class button_state { pressed, released };
enum class key_state { pressed, released, repeat };

enum class input_event_type {
  mouse_button,
  mouse_move,
  cursor,
  mouse_scroll,
  key,
  stylus_proximity,
  stylus_properties,
  gamepad_button,
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
  button_state state;
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

struct gamepad_axis 
{
	float x;
	float y;
};

struct gamepad_state
{
	std::vector<button_state> buttons;
	std::vector<gamepad_axis> axis;
};


/////////////////////////////////////////////
// Translated inputs
// Commands (events) & values
//
// Command trigger: button press, gamepad axis trigger
//	=> polling + events
// Value mapping (1D/2D) w/ deadzone: gamepad axis, d-pad, 4-key set (arrows, WASD, others), 8-key set (numpad)
//	=> polling only 
// Value (boolean): button press
//
// Must be serializable

class state 
{
public:

private:
	gamepad_state gpstate[max_gamepads];
};

class action : public observable<>
{
public:
private:
	static action mouse_button(uint32_t key_code, /*callback*/);
};



class game_inputs 
{
public:
	game_inputs(...)
	{
		jump = 
	}

private:
	action jump;
	action attack;
};


}