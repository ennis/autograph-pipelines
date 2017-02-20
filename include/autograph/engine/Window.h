#pragma once
#include <autograph/engine/Application.h>
#include <autograph/engine/Input.h>

struct GLFWwindow;
struct GLFWpointerevent;

namespace ag {
//////////////////////////////////////////////
enum class AG_REFLECT EventType {
  MouseButton = 0,
  MouseMove,
  Cursor,
  CursorEnter,
  CursorExit,
  MouseScroll,
  Key,
  Text,
  StylusProximity,
  StylusProperties,
  WindowResize,
  PointerEnter,
  PointerDown,
  PointerUp,
  PointerMove,
  PointerLeave
};

//////////////////////////////////////////////
struct AG_API CursorEnterEvent {};
struct AG_API CursorExitEvent {};

//////////////////////////////////////////////
struct AG_API WindowResizeEvent {
  int width;
  int height;
};

//////////////////////////////////////////////
struct AG_API MouseButtonEvent {
  int button;
  ButtonState action;
};

//////////////////////////////////////////////
struct AG_API CursorEvent {
  // in client units (pixels)
  int x;
  int y;
  double xFloat;
  double yFloat;
};

//////////////////////////////////////////////
struct AG_API MouseMoveEvent {
  double dx;
  double dy;
};

//////////////////////////////////////////////
struct AG_API MouseScrollEvent {
  double dx;
  double dy;
};

//////////////////////////////////////////////
struct AG_API KeyEvent {
  int key;
  int scancode;
  KeyState action;
};

//////////////////////////////////////////////
struct AG_API TextEvent {
  char32_t codepoint;
};

//////////////////////////////////////////////
struct AG_API StylusProximityEvent {
  // TODO
};

//////////////////////////////////////////////
struct AG_API StylusPropertiesEvent {
  double x;
  double y;
  double pressure;
  double tilt;
};

//////////////////////////////////////////////
struct AG_API PointerInfo {
  int id;
  int type;
  int button;
  uint32_t buttons;
  double x;
  double y;
  int mask;
  double pressure; // == 0.0 if stylus not touching
  double tiltX;
  double tiltY;
};

struct AG_API PointerEvent {
  PointerInfo info;
};

// Window events
// ripoff of SFML event class
struct Event {
  Event(EventType ty) : type{ty} {}
  EventType type;

  union {
    MouseButtonEvent mouseButton;
    MouseMoveEvent mouseMove;
    CursorEvent cursor;
    MouseScrollEvent scroll;
    KeyEvent key;
    TextEvent text;
    StylusProximityEvent stylusProximity;
    StylusPropertiesEvent stylusProperties;
    WindowResizeEvent resize;
    CursorEnterEvent cursorEnter;
    CursorExitEvent cursorExit;
    PointerEvent pointer;
  };
};

// A rendering window
class AG_API Window {
public:
  Window(int w, int h, const char *title);
  ~Window();

  ivec2 getFramebufferSize();
  ivec2 getWindowSize();
  ivec2 getCursorPosition();
  KeyState getKey(int key);

  void onRender(std::function<void(Window &, double)> renderFunc) {
    renderFunc_ = std::move(renderFunc);
  }
  void onEvent(std::function<void(Window &, const Event &)> eventFunc) {
    eventFunc_ = std::move(eventFunc);
  }
  // will end show()
  void close();
  void show();

private:
  void mouseButtonHandler(int button, int action, int mods);
  void cursorPosHandler(double xpos, double ypos);
  void cursorEnterHandler(int entered);
  void scrollHandler(double xoffset, double yoffset);
  void keyHandler(int key, int scancode, int action, int mods);
  void charHandler(unsigned int codepoint);
  void windowSizeHandler(int width, int height);
  void pointerEventHandler(const GLFWpointerevent *);

  static void MouseButtonHandler(GLFWwindow *window, int button, int action,
                                 int mods);
  static void CursorPosHandler(GLFWwindow *window, double xpos, double ypos);
  static void CursorEnterHandler(GLFWwindow *window, int entered);
  static void ScrollHandler(GLFWwindow *window, double xoffset, double yoffset);
  static void KeyHandler(GLFWwindow *window, int key, int scancode, int action,
                         int mods);
  static void CharHandler(GLFWwindow *window, unsigned int codepoint);
  static void WindowSizeHandler(GLFWwindow *window, int width, int height);
  static void PointerEventHandler(GLFWwindow *window, const GLFWpointerevent *);

  std::function<void(Window &, double)> renderFunc_;
  std::function<void(Window &, const Event &)> eventFunc_;
  GLFWwindow *window_;
  bool profiler_ = false;
  bool showDebugOverlay_ = false;
};
}