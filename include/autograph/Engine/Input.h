#pragma once
#include <autograph/Core/Types.h>
#include <autograph/Engine/Exports.h>
#include <autograph/Engine/Observable.h>
#include <cstdint>
#include <array>

namespace ag 
{
/// @defgroup input Input API
//////////////////////////////////////////////

/**
 * @brief Key action
 * @details [TODO notes about key repeat events]
 * @ingroup input
 * 
 */
enum class KeyAction { Press=0, Release, Repeat };
enum class KeyState { Pressed = 0, Released };


//////////////////////////////////////////////
struct GamepadAxis {
	float x;
	float y;
};

//////////////////////////////////////////////
struct GamepadState {
  static constexpr int kMaxButtons = 32;
  static constexpr int kMaxAxes = 8;
  std::array<KeyState, kMaxButtons> buttons;
  std::array<GamepadAxis, kMaxAxes> axis;
};

//////////////////////////////////////////////
class AG_ENGINE_API Action : public Observable<> {
public:
  Action() {}
  virtual ~Action() {}

protected:
};

//////////////////////////////////////////////
/*class AG_ENGINE_API KeyAction : public Action {
public:
  KeyAction(int key);
  ~KeyAction();

private:
  Subscription sub_;
  int key_;
};*/

//////////////////////////////////////////////
/*class AG_ENGINE_API GamepadButtonAction : public Action {
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
};*/

}