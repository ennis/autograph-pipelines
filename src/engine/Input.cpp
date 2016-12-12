#include <autograph/engine/Input.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#ifdef WIN32
#include <Windows.h>
#include <Xinput.h>
#include <winerror.h>
#endif
#include <autograph/support/Debug.h>

namespace ag {
// synchronous stream of input events
Observable<InputEvent *> onInputEvent;
Observable<> onPollInput;

namespace {
GLFWwindow *gInputWindow;

// GLFW event handlers
void GLFWMouseButtonHandler(GLFWwindow *window, int button, int action,
                            int mods) {
	ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
  MouseButtonEvent mb;
  mb.button = button;
  mb.state =
      action == GLFW_PRESS ? ButtonState::Pressed : ButtonState::Released;
  // input::events stream is synchronous
  onInputEvent(&mb);
}

void GLFWCursorPosHandler(GLFWwindow *window, double xpos, double ypos) {
  CursorEvent cur;
  cur.position = ag::ivec2{(int)xpos, (int)ypos};
  onInputEvent(&cur);
}

void GLFWCursorEnterHandler(GLFWwindow *window, int entered) {
  // instance->on_cursor_enter(window, entered);
}

void GLFWScrollHandler(GLFWwindow *window, double xoffset, double yoffset) {
	ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
  // instance->on_scroll(window, xoffset, yoffset);
}

void GLFWKeyHandler(GLFWwindow *window, int key, int scancode, int action,
                    int mods) {
	ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
  // instance->on_key(window, key, scancode, action, mods);
	KeyEvent ev;
	ev.code = key;
	ev.state = action == GLFW_PRESS ? KeyState::Pressed : KeyState::Released;
	onInputEvent(&ev);
}

void GLFWCharHandler(GLFWwindow *window, unsigned int codepoint) {
	ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
  TextEvent text;
  text.codepoint = codepoint;
  onInputEvent(&text);
  // instance->on_char(window, codepoint);
}
}

KeyAction::KeyAction(int key) : key_{key} {
  onInputEvent.subscribe(sub_, [this](InputEvent *ev) {
    if (ev->type == InputEventType::Key) {
      if (this->key_ == static_cast<KeyEvent *>(ev)->code) {
        operator()();
      }
    }
  });
}

KeyAction::~KeyAction() { sub_.unsubscribe(); }

GamepadButtonAction::GamepadButtonAction(uint16_t button, bool autofire,
                                             float fire_rate)
    : autofire_{ autofire }, fire_rate_{ fire_rate }, button_{ button } 
{
  onPollInput.subscribe(sub_, [this]() {
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
  });
}

GamepadButtonAction::~GamepadButtonAction() { sub_.unsubscribe(); }

void initializeInput(GLFWwindow *window) {
  gInputWindow = window;
  glfwSetCharCallback(window, GLFWCharHandler);
  glfwSetCursorEnterCallback(window, GLFWCursorEnterHandler);
  glfwSetCursorPosCallback(window, GLFWCursorPosHandler);
  glfwSetKeyCallback(window, GLFWKeyHandler);
  glfwSetMouseButtonCallback(window, GLFWMouseButtonHandler);
  glfwSetScrollCallback(window, GLFWScrollHandler);
}

void processInput() { 
	onPollInput(); 
}

AG_API ivec2 getCursorPosition() {
  double x, y;
  glfwGetCursorPos(gInputWindow, &x, &y);
  return ivec2{(int)x, (int)y};
}

}
