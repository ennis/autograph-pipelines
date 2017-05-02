#include <autograph/Window/WindowGLFW.h>
#include <autograph/Core/Support/Debug.h>
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace ag {

// GLFW event handlers
void WindowGLFW::MouseButtonHandler(GLFWwindow *window, int button, int action,
                                    int mods) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->mouseButtonHandler(button, action,
                                                           mods);
}

void WindowGLFW::CursorPosHandler(GLFWwindow *window, double xpos,
                                  double ypos) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->cursorPosHandler(xpos, ypos);
}

void WindowGLFW::CursorEnterHandler(GLFWwindow *window, int entered) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->cursorEnterHandler(entered);
}

void WindowGLFW::ScrollHandler(GLFWwindow *window, double xoffset,
                               double yoffset) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->scrollHandler(xoffset, yoffset);
}

void WindowGLFW::KeyHandler(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->keyHandler(key, scancode, action, mods);
}

void WindowGLFW::CharHandler(GLFWwindow *window, unsigned int codepoint) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->charHandler(codepoint);
}

void WindowGLFW::WindowSizeHandler(GLFWwindow *window, int width, int height) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->windowSizeHandler(width, height);
}

void WindowGLFW::PointerEventHandler(GLFWwindow *window,
                                     const GLFWpointerevent *pointerEvent) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<WindowGLFW *>(userptr)->pointerEventHandler(pointerEvent);
}

void WindowGLFW::mouseButtonHandler(int button, int action, int mods) {
  Event ev{EventType::MouseButton};
  ev.mouseButton.button = button;
  ev.mouseButton.action =
      (action == GLFW_PRESS) ? KeyAction::Press : KeyAction::Release;
  processEvent(ev);
}

void WindowGLFW::cursorPosHandler(double xpos, double ypos) {
  Event ev{EventType::Cursor};
  ev.cursor.xFloat = xpos;
  ev.cursor.yFloat = ypos;
  ev.cursor.x = (int)xpos;
  ev.cursor.y = (int)ypos;
  processEvent(ev);
}

void WindowGLFW::cursorEnterHandler(int entered) {
  Event ev{entered ? EventType::CursorEnter : EventType::CursorExit};
  processEvent(ev);
}

void WindowGLFW::scrollHandler(double xoffset, double yoffset) {
  Event ev{EventType::MouseScroll};
  ev.scroll.dx = xoffset;
  ev.scroll.dy = yoffset;
  processEvent(ev);
}

int GLFWKeyToAutographKey(int key) {
  return key; // we use the same key mappings as GLFW internally
}
int GLFWModsToAutographMods(int mods) {
  return mods; // we use the same key modifiers mappings as GLFW internally
}

void WindowGLFW::keyHandler(int key, int scancode, int action, int mods) {
  Event ev{EventType::Key};
  ev.key.action =
      (action == GLFW_PRESS)
          ? KeyAction::Press
          : (action == GLFW_REPEAT ? KeyAction::Repeat : KeyAction::Release);
  ev.key.scancode = scancode;
  ev.key.key = GLFWKeyToAutographKey(key);
  ev.key.mods = GLFWModsToAutographMods(mods);
  processEvent(ev);

  // Debug hooks
  /*if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_F12 && action == GLFW_PRESS)
  {
    // Toggle debug overlay
    showDebugOverlay_ = !showDebugOverlay_;
  }
  if (mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT) && key == GLFW_KEY_F12 &&
      action == GLFW_PRESS) {
    // capture next frame
    setNextFrameCapture();
  }
  if (mods == (GLFW_MOD_CONTROL) && key == GLFW_KEY_F8 &&
      action == GLFW_PRESS) {
    // toggle profiler
    profiler_ = !profiler_;
    AG_DEBUG("Profiler {}", profiler_ ? "ON" : "OFF");
  }
  if (mods == (GLFW_MOD_CONTROL) && key == GLFW_KEY_F5 &&
      action == GLFW_PRESS) {
    // reload all shaders
    getPipelineStateCache().reloadAll();
  }*/
}

void WindowGLFW::charHandler(unsigned int codepoint) {
  Event ev{EventType::Text};
  ev.text.codepoint = codepoint;
  processEvent(ev);
}

void WindowGLFW::windowSizeHandler(int width, int height) {
  Event ev{EventType::WindowResize};
  ev.resize.width = width;
  ev.resize.height = height;
  processEvent(ev);
}

void WindowGLFW::pointerEventHandler(const GLFWpointerevent *pointerevent) {
  EventType type = EventType::PointerMove;
  switch (pointerevent->action) {
  case 1:
    type = EventType::PointerUp;
    break;
  case 2:
    type = EventType::PointerDown;
    break;
  case 3:
    type = EventType::PointerEnter;
    break;
  case 4:
    type = EventType::PointerLeave;
    break;
  case 5:
    type = EventType::PointerMove;
    break;
  };
  Event ev{type};
  ev.pointer.info.button = pointerevent->button;
  ev.pointer.info.buttons = pointerevent->buttons;
  ev.pointer.info.id = pointerevent->id;
  ev.pointer.info.mask = pointerevent->mask;
  ev.pointer.info.type = pointerevent->type;
  ev.pointer.info.x = pointerevent->x;
  ev.pointer.info.y = pointerevent->y;
  ev.pointer.info.pressure = pointerevent->pressure;
  ev.pointer.info.tiltX = pointerevent->tiltX;
  ev.pointer.info.tiltY = pointerevent->tiltY;
  processEvent(ev);
}

WindowGLFW::WindowGLFW(int w, int h, const char *title) {
  if (!glfwInit())
    ag::failWith("Application failed to initialize (glfwInit)");

  // Create a windowed mode window and its OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, gl::TRUE_);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, gl::TRUE_);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  w_ = glfwCreateWindow(w, h, title, NULL, NULL);
  if (!w_) {
    glfwTerminate();
    ag::failWith("Application failed to initialize (glfwCreateWindow)");
  }
  glfwSetWindowUserPointer(w_, this);
  glfwMakeContextCurrent(w_);
  if (!gl::sys::LoadFunctions()) {
    glfwTerminate();
    ag::failWith("Application failed to initialize (ogl_LoadFunctions)");
  }
  glfwSwapInterval(1);

  // set event handlers
  glfwSetWindowSizeCallback(w_, WindowSizeHandler);
  glfwSetCursorEnterCallback(w_, CursorEnterHandler);
  glfwSetMouseButtonCallback(w_, MouseButtonHandler);
  glfwSetScrollCallback(w_, ScrollHandler);
  glfwSetCursorPosCallback(w_, CursorPosHandler);
  glfwSetCharCallback(w_, CharHandler);
  glfwSetKeyCallback(w_, KeyHandler);
  glfwSetPointerEventCallback(w_, PointerEventHandler);

#ifdef WIN32
  EnableMouseInPointer(true);
#endif
}

void WindowGLFW::unsetEventCallback() { eventCallback_ = {}; }

void WindowGLFW::setEventCallback(
    std::function<void(const Event &, Window *)> callback) {
  eventCallback_ = std::move(callback);
}

void WindowGLFW::processEvent(const Event &e) {
  if (eventCallback_)
    eventCallback_(e, this);
}

bool WindowGLFW::isFocused() { return !!glfwGetWindowAttrib(w_, GLFW_FOCUSED); }

void WindowGLFW::swapBuffers() { glfwSwapBuffers(w_); }

void WindowGLFW::prepareForRender() { glfwMakeContextCurrent(w_); }

// Framebuffer &WindowGLFW::getFramebuffer() {}

ivec2 WindowGLFW::getCursorPos() {
  double x, y;
  glfwGetCursorPos(w_, &x, &y);
  return ivec2{(int)x, (int)y};
}

ivec2 WindowGLFW::getFramebufferSize() {
  int w, h;
  glfwGetWindowSize(w_, &w, &h);
  return ivec2{w, h};
}

ivec2 WindowGLFW::getWindowSize() {
  int w, h;
  glfwGetWindowSize(w_, &w, &h);
  return ivec2{w, h};
}

} // namespace ag