#pragma once
#include <cstdint>
#include <array>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <autograph/Types.h>
#include <autograph/engine/Observable.h>

namespace ag 
{
/// @defgroup input Input API

//////////////////////////////////////////////

/**
 * @brief Mouse button state
 * @details 
 * @ingroup input
 * 
 */
enum class ButtonState { Pressed, Released };

/**
 * @brief Key state
 * @details [TODO notes about key repeat events]
 * @ingroup input
 * 
 */
enum class KeyState { Pressed, Released, Repeat };

//////////////////////////////////////////////
/**
 * @brief The derived type of an InputEvent
 * @details 
 * @ingroup input
 * 
 */
enum class InputEventType {
  MouseButton,
  MouseMove,
  Cursor,
  MouseScroll,
  Key,
  Text,
  StylusProximity,
  StylusProperties
};

//////////////////////////////////////////////

/**
 * @brief Base class for input event
 * @details Actual input events are derived from this class. Use 
 * @ingroup input
 * 
 */
struct InputEvent {
  InputEvent(InputEventType ty) : type{ty} {}

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

  InputEventType type;
};

template <InputEventType Type> struct InputEventT : public InputEvent {
  static constexpr InputEventType event_type = Type;
  InputEventT() : InputEvent{Type} {}
};

//////////////////////////////////////////////
struct MouseButtonEvent
    : public InputEventT<InputEventType::MouseButton> {
  unsigned button;
  ButtonState state;
};

//////////////////////////////////////////////
struct CursorEvent : public InputEventT<InputEventType::Cursor> {
  // in client units (pixels)
  ivec2 position;
};

//////////////////////////////////////////////
struct MouseMoveEvent : public InputEventT<InputEventType::MouseMove> {
  ivec2 delta;
};

//////////////////////////////////////////////
struct MouseScrollEvent
    : public InputEventT<InputEventType::MouseScroll> {
  ivec2 delta;
};

//////////////////////////////////////////////
struct KeyEvent : public InputEventT<InputEventType::Key> {
  uint32_t code;
  KeyState state;
};

//////////////////////////////////////////////
struct TextEvent : public InputEventT<InputEventType::Text> {
	uint32_t codepoint;
};

//////////////////////////////////////////////
struct StylusProximityEvent
    : public InputEventT<InputEventType::StylusProximity> {
};

//////////////////////////////////////////////
struct StylusPropertiesEvent
    : public InputEventT<InputEventType::StylusProperties> {
  vec2 position;
  double pressure;
  double tilt;
};

//////////////////////////////////////////////
struct GamepadAxis {
	vec2 position;
};

//////////////////////////////////////////////
struct GamepadState {
  static constexpr int kMaxButtons = 32;
  static constexpr int kMaxAxes = 8;
  std::array<ButtonState, kMaxButtons> buttons;
  std::array<GamepadAxis, kMaxAxes> axis;
};

//////////////////////////////////////////////
AG_API extern Observable<InputEvent*> onInputEvent;
AG_API extern Observable<> onPollInput;

AG_API void processInput();


//////////////////////////////////////////////
class AG_API Action : public Observable<> {
public:
  Action() {}
  virtual ~Action() {}

protected:
};

//////////////////////////////////////////////
class AG_API KeyAction : public Action {
public:
  KeyAction(int key);
  ~KeyAction();

private:
  Subscription sub_;
  int key_;
};

//////////////////////////////////////////////
class AG_API GamepadButtonAction : public Action {
public:
  GamepadButtonAction(uint16_t button, bool autofire = false,
                        float fire_rate = 0.0f);
  ~GamepadButtonAction();

private:
  Subscription sub_;
  bool autofire_;
  float fire_rate_;
  uint16_t button_;
  uint32_t prev_packet_{0};
  ButtonState prev_state_{ButtonState::Released};
};

//////////////////////////////////////////////
AG_API ivec2 getCursorPosition();

}