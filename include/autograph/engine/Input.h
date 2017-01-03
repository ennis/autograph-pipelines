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
struct GamepadAxis {
	float x;
	float y;
};

//////////////////////////////////////////////
struct GamepadState {
  static constexpr int kMaxButtons = 32;
  static constexpr int kMaxAxes = 8;
  std::array<ButtonState, kMaxButtons> buttons;
  std::array<GamepadAxis, kMaxAxes> axis;
};

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

}