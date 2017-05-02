#pragma once
#include <autograph/Engine/Engine.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace ag {
class WindowGLFW : public Window {
public:
  WindowGLFW(int w, int h, const char *title);

  virtual bool isFocused() override;
  virtual ivec2 getCursorPos() override;
  virtual ivec2 getFramebufferSize() override;
  virtual ivec2 getWindowSize() override;
  virtual void unsetEventCallback() override;
  virtual void setEventCallback(
      std::function<void(const Event &, Window *)> callback) override;
  virtual void swapBuffers() override;
  virtual void prepareForRender() override;
  // virtual Framebuffer &getFramebuffer() override;
  void processEvent(const Event &e);
  GLFWwindow *getGLFWwindow() { return w_; }

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

  std::function<void(const Event &, Window *)> eventCallback_;
  GLFWwindow *w_;
};

} // namespace ag