#include <GLFW/glfw3.h>
#include <autograph/engine/Input.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#ifdef WIN32
#include <Windows.h>
#include <Xinput.h>
#include <winerror.h>
#endif
#include <autograph/support/Debug.h>

namespace ag {

KeyAction::KeyAction(int key) : key_{key} {
  /*onInputEvent.subscribe(sub_, [this](InputEvent *ev) {
          if (ev->type == InputEventType::Key) {
                  if (this->key_ == static_cast<KeyEvent *>(ev)->code) {
                          operator()();
                  }
          }
  });*/
}

KeyAction::~KeyAction() { sub_.unsubscribe(); }

GamepadButtonAction::GamepadButtonAction(uint16_t button, bool autofire,
                                         float fire_rate)
    : autofire_{autofire}, fire_rate_{fire_rate}, button_{button} {
// onPollInput.subscribe(sub_, [this]() {
#if 0
    XINPUT_STATE xis;
    auto result = XInputGetState(0, &xis);
    if (result != ERROR_SUCCESS)
      return;
    if (xis.dwPacketNumber == this->prev_packet_ && !this->autofire_)
      return;
    this->prev_packet_ = xis.dwPacketNumber;
    if (xis.Gamepad.wButtons & this->button_) {
      auto prev = this->prev_state_;
      this->prev_state_ = ButtonState::Pressed;
      if ((prev == ButtonState::Released) || this->autofire_) {
        AG_DEBUG("Controller 0 button {} pressed\n",
                   this->button_);
        operator()();
      }
    } else
      this->prev_state_ = ButtonState::Released;
#endif
  //});
}

GamepadButtonAction::~GamepadButtonAction() { sub_.unsubscribe(); }
}
