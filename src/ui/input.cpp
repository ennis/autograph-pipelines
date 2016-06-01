#include "input.hpp"
#include <GLFW/glfw3.h>
#ifdef WIN32
#include <Windows.h>
#include <Xinput.h>
#include <winerror.h>
#endif
#include <cppformat/format.h>
#include <iostream>

namespace input {
// synchronous stream of input events
observable<input_event *> events;
observable<> poll;

namespace {
GLFWwindow *g_input_window;

// GLFW event handlers
void GLFWMouseButtonHandler(GLFWwindow *window, int button, int action,
                            int mods) {
  mouse_button_event mb;
  mb.button = button;
  mb.state =
      action == GLFW_PRESS ? button_state::pressed : button_state::released;
  // input::events stream is synchronous
  events.signal(&mb);
}

void GLFWCursorPosHandler(GLFWwindow *window, double xpos, double ypos) {
  cursor_event cur;
  cur.pos = glm::ivec2{(int)xpos, (int)ypos};
  events.signal(&cur);
}

void GLFWCursorEnterHandler(GLFWwindow *window, int entered) {
  // instance->on_cursor_enter(window, entered);
}

void GLFWScrollHandler(GLFWwindow *window, double xoffset, double yoffset) {
  // instance->on_scroll(window, xoffset, yoffset);
}

void GLFWKeyHandler(GLFWwindow *window, int key, int scancode, int action,
                    int mods) {
  // instance->on_key(window, key, scancode, action, mods);
}

void GLFWCharHandler(GLFWwindow *window, unsigned int codepoint) {
  text_event text;
  text.codepoint = codepoint;
  events.signal(&text);
  // instance->on_char(window, codepoint);
}
}

key_action::key_action(int key) : key_{key} {
  events.subscribe(sub_, [this](input_event *ev) {
    if (ev->type == input_event_type::key) {
      if (this->key_ == static_cast<key_event *>(ev)->code) {
        signal();
      }
    }
  });
}

key_action::~key_action() { sub_.unsubscribe(); }

gamepad_button_action::gamepad_button_action(uint16_t button, bool autofire,
                                             float fire_rate)
    : button_{button}, autofire_{autofire}, fire_rate_{fire_rate} {
  poll.subscribe(sub_, [this]() {
#ifdef WIN32
    XINPUT_STATE xis;
    auto result = XInputGetState(0, &xis);
    if (result != ERROR_SUCCESS)
      return;
    if (xis.dwPacketNumber == this->prev_packet_ && !this->autofire_)
      return;
    this->prev_packet_ = xis.dwPacketNumber;
    if (xis.Gamepad.wButtons & this->button_) {
      auto prev = this->prev_state_;
      this->prev_state_ = button_state::pressed;
      if ((prev == button_state::released) || this->autofire_) {
        fmt::print(std::clog, "Controller 0 button {} pressed\n",
                   this->button_);
        signal();
      }
    } else
      this->prev_state_ = button_state::released;
#endif
  });
}

gamepad_button_action::~gamepad_button_action() { sub_.unsubscribe(); }

void initialize(GLFWwindow *window) {
  g_input_window = window;
  glfwSetCharCallback(window, GLFWCharHandler);
  glfwSetCursorEnterCallback(window, GLFWCursorEnterHandler);
  glfwSetCursorPosCallback(window, GLFWCursorPosHandler);
  glfwSetKeyCallback(window, GLFWKeyHandler);
  glfwSetMouseButtonCallback(window, GLFWMouseButtonHandler);
  glfwSetScrollCallback(window, GLFWScrollHandler);
}

void process_input() { poll.signal(); }

glm::ivec2 cursor_pos() {
  double x, y;
  glfwGetCursorPos(g_input_window, &x, &y);
  return glm::ivec2{(int)x, (int)y};
}

}
