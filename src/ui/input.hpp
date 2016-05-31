#pragma once
#include "../observable.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct GLFWwindow;

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
  text,
  stylus_proximity,
  stylus_properties
};

struct input_event {
  using ptr = std::shared_ptr<input_event>;
  input_event(input_event_type ty) : type{ty} {}

  template <typename T> T *as() {
    if (T::event_type == type)
      return static_cast<T *>(this);
    return nullptr;
  }

  template <typename T> const T *as() const {
    if (T::event_type == type)
      return static_cast<const T *>(this);
    return nullptr;
  }

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
  glm::ivec2 pos;
};

struct mouse_move_event : public t_input_event<input_event_type::mouse_move> {
  using ptr = std::shared_ptr<mouse_move_event>;
  glm::ivec2 delta;
};

struct mouse_scroll_event
    : public t_input_event<input_event_type::mouse_scroll> {
  using ptr = std::shared_ptr<mouse_scroll_event>;
  glm::ivec2 delta;
};

struct key_event : public t_input_event<input_event_type::key> {
  using ptr = std::shared_ptr<key_event>;
  uint32_t code;
  key_state state;
};

struct text_event : public t_input_event<input_event_type::text> {
	uint32_t codepoint;
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
  glm::vec2 pos;
  double pressure;
  double tilt;
};

struct gamepad_axis {
	glm::vec2 pos;
};

struct gamepad_state {
  std::vector<button_state> buttons;
  std::vector<gamepad_axis> axis;
};

extern observable<input_event*> events;

/////////////////////////////////////////////
// Translated inputs
// Commands (events) & values
//
// Command trigger: button press, gamepad axis trigger
//	=> polling + events
// Value mapping (1D/2D) w/ deadzone: gamepad axis, d-pad, 4-key set (arrows,
// WASD, others), 8-key set (numpad)
//	=> polling only
// Value (boolean): button press
//
// Must be serializable

class action : public observable<> {
public:
  action() {}
  virtual ~action() {}

protected:
};

class key_action : public action {
public:
  key_action(int key);
  ~key_action();

private:
  subscription sub_;
  int key_;
};

class gamepad_button_action : public action {
public:
  gamepad_button_action(uint16_t button, bool autofire = false,
                        float fire_rate = 0.0f);
  ~gamepad_button_action();

private:
  subscription sub_;
  bool autofire_;
  float fire_rate_;
  uint16_t button_;
  uint32_t prev_packet_{0};
  button_state prev_state_{button_state::released};
};

void initialize(GLFWwindow *window);
void process_input();
glm::ivec2 cursor_pos();

}