#include <autograph/engine/DebugOverlay.h>
#include <autograph/engine/Window.h>
#include <autograph/engine/ImGuiUtils.h>
#include <autograph/gl/Capture.h>
#include <autograph/gl/Device.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <gl_core_4_5.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_icons.h>
#include <imgui_impl_glfw_gl3.h>
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace ag {

static void ensureImGuiSetup() {
  static bool setup = false;
  if (!setup) {
    // setup styles
    ImGuiIO &io = ImGui::GetIO();
    // io.Fonts->AddFontDefault();
    auto fontFile = ag::getActualPath("resources/fonts/Cousine-Regular.ttf");
    io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 14.0f);
    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    fontFile = ag::getActualPath("resources/fonts/fontawesome-webfont.ttf");
    ImFontConfig config;
    config.PixelSnapH = true;
    config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 14.0f, &config,
                                 icons_ranges);

    ImGuiStyle &style = ImGui::GetStyle();
    style.ChildWindowRounding = 3.f;
    style.GrabRounding = 0.f;
    style.WindowRounding = 0.f;
    style.ScrollbarRounding = 3.f;
    style.FrameRounding = 3.f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.91f, 0.91f, 0.91f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.75f, 0.42f, 0.02f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_ComboBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.99f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.42f, 0.02f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive] =
        ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.75f, 0.42f, 0.02f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.94f, 0.47f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.75f, 0.42f, 0.02f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_Column] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] =
        ImVec4(0.75f, 0.42f, 0.02f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] =
        ImVec4(0.75f, 0.42f, 0.02f, 0.95f);
    style.Colors[ImGuiCol_CloseButton] = ImVec4(0.42f, 0.42f, 0.42f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered] =
        ImVec4(0.02f, 0.61f, 0.64f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive] =
        ImVec4(0.02f, 0.61f, 0.64f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(0.00f, 0.57f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.10f, 0.30f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(0.00f, 0.40f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.75f, 0.42f, 0.02f, 0.35f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
    style.Colors[ImGuiCol_ModalWindowDarkening] =
        ImVec4(0.06f, 0.06f, 0.06f, 0.35f);
  }
}

// GLFW event handlers
void Window::MouseButtonHandler(GLFWwindow *window, int button, int action,
                                int mods) {
  ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->mouseButtonHandler(button, action, mods);
}

void Window::CursorPosHandler(GLFWwindow *window, double xpos, double ypos) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->cursorPosHandler(xpos, ypos);
}

void Window::CursorEnterHandler(GLFWwindow *window, int entered) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->cursorEnterHandler(entered);
}

void Window::ScrollHandler(GLFWwindow *window, double xoffset, double yoffset) {
  ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->scrollHandler(xoffset, yoffset);
}

void Window::KeyHandler(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->keyHandler(key, scancode, action, mods);
}

void Window::CharHandler(GLFWwindow *window, unsigned int codepoint) {
  ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->charHandler(codepoint);
}

void Window::WindowSizeHandler(GLFWwindow *window, int width, int height) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->windowSizeHandler(width, height);
}

void Window::PointerEventHandler(GLFWwindow *window,
                                 const GLFWpointerevent *pointerEvent) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->pointerEventHandler(pointerEvent);
}

void Window::mouseButtonHandler(int button, int action, int mods) {
  Event ev{EventType::MouseButton};
  ev.mouseButton.button = button;
  ev.mouseButton.action =
      (action == GLFW_PRESS) ? ButtonState::Pressed : ButtonState::Released;
  if (eventFunc_)
    eventFunc_(*this, ev);
}

void Window::cursorPosHandler(double xpos, double ypos) {
  Event ev{EventType::Cursor};
  ev.cursor.xFloat = xpos;
  ev.cursor.yFloat = ypos;
  ev.cursor.x = (int)xpos;
  ev.cursor.y = (int)ypos;
  if (eventFunc_)
    eventFunc_(*this, ev);
}

void Window::cursorEnterHandler(int entered) {
  Event ev{entered ? EventType::CursorEnter : EventType::CursorExit};
  if (eventFunc_)
    eventFunc_(*this, ev);
}

void Window::scrollHandler(double xoffset, double yoffset) {
  Event ev{EventType::MouseScroll};
  ev.scroll.dx = xoffset;
  ev.scroll.dy = yoffset;
  if (eventFunc_)
    eventFunc_(*this, ev);
}

void Window::keyHandler(int key, int scancode, int action, int mods) {
  Event ev{EventType::Key};
  ev.key.action =
      (action == GLFW_PRESS)
          ? KeyState::Pressed
          : (action == GLFW_REPEAT ? KeyState::Repeat : KeyState::Released);
  ev.key.scancode = scancode;
  ev.key.key = key;
  if (eventFunc_)
    eventFunc_(*this, ev);
  // Debug hooks
  if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_F12 && action == GLFW_PRESS) {
    // Toggle debug overlay
    showDebugOverlay_ = !showDebugOverlay_;
  }
  if (mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT) && key == GLFW_KEY_F12 &&
      action == GLFW_PRESS) {
    // capture next frame
    gl::setNextFrameCapture();
  }
}

void Window::charHandler(unsigned int codepoint) {
  Event ev{EventType::Text};
  ev.text.codepoint = codepoint;
  if (eventFunc_)
    eventFunc_(*this, ev);
}

void Window::windowSizeHandler(int width, int height) {
  Event ev{EventType::WindowResize};
  ev.resize.width = width;
  ev.resize.height = height;
  if (eventFunc_)
    eventFunc_(*this, ev);
}

void Window::pointerEventHandler(const GLFWpointerevent *pointerevent) {
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
  if (eventFunc_)
    eventFunc_(*this, ev);
}

KeyState Window::getKey(int key) {
  auto state = glfwGetKey(window_, key);
  return (state == GLFW_PRESS) ? KeyState::Pressed : KeyState::Released;
}

Window::Window(int w, int h, const char *title) {
  if (!glfwInit())
    throw std::runtime_error("Application failed to initialize (glfwInit)");

  // Create a windowed mode window and its OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  window_ = glfwCreateWindow(w, h, title, NULL, NULL);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error(
        "Application failed to initialize (glfwCreateWindow)");
  }
  glfwSetWindowUserPointer(window_, this);

  glfwMakeContextCurrent(window_);
  if (!ogl_LoadFunctions()) {
    glfwTerminate();
    throw std::runtime_error(
        "Application failed to initialize (ogl_LoadFunctions)");
  }
  glfwSwapInterval(1);

  // ImGui init
  ImGui_ImplGlfwGL3_Init(window_, false);
  ensureImGuiSetup();

  // set event handlers
  glfwSetWindowSizeCallback(window_, WindowSizeHandler);
  glfwSetCursorEnterCallback(window_, CursorEnterHandler);
  glfwSetMouseButtonCallback(window_, MouseButtonHandler);
  glfwSetScrollCallback(window_, ScrollHandler);
  glfwSetCursorPosCallback(window_, CursorPosHandler);
  glfwSetCharCallback(window_, CharHandler);
  glfwSetKeyCallback(window_, KeyHandler);
  glfwSetPointerEventCallback(window_, PointerEventHandler);

  // autograph init
  gl::DeviceConfig devCfg;
  devCfg.init_fb_width = w;
  devCfg.init_fb_height = h;
  devCfg.max_frames_in_flight = 3;
  gl::initialize(devCfg);
  gl::resizeDefaultFramebuffer(w, h);

#ifdef WIN32
  EnableMouseInPointer(true);
#endif
}

Window::~Window() {
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();
}

ivec2 Window::getCursorPosition() {
  double x, y;
  glfwGetCursorPos(window_, &x, &y);
  return ivec2{(int)x, (int)y};
}

ivec2 Window::getFramebufferSize() {
  int w, h;
  glfwGetWindowSize(window_, &w, &h);
  return ivec2{w, h};
}

ivec2 Window::getWindowSize() {
  int w, h;
  glfwGetWindowSize(window_, &w, &h);
  return ivec2{w, h};
}

void Window::show() {
  double tlast = glfwGetTime();
  ImGui_ImplGlfwGL3_NewFrame();
  while (!glfwWindowShouldClose(window_)) {
    auto framebufferSize = getFramebufferSize();
    ag::gl::resizeDefaultFramebuffer(framebufferSize.x, framebufferSize.y);
    double t = glfwGetTime();
    double dt = t - tlast;
    tlast = t;
    gui::beginFrame();
    if (renderFunc_)
      renderFunc_(*this, dt);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_STENCIL_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (showDebugOverlay_) {
      drawDebugOverlay(dt);
    }
    gui::endFrame();
    ImGui::Render();
    ImGui_ImplGlfwGL3_NewFrame();
    ag::gl::endFrame();
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Window::close() { glfwSetWindowShouldClose(window_, 1); }
}
