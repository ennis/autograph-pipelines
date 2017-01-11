#include <autograph/engine/Window.h>
#include <autograph/gl/Device.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <gl_core_4_5.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace ag {
// GLFW event handlers
void Window::MouseButtonHandler(GLFWwindow *window, int button, int action,
                                int mods) {
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
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->scrollHandler(xoffset, yoffset);
}

void Window::KeyHandler(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->keyHandler(key, scancode, action, mods);
}

void Window::CharHandler(GLFWwindow *window, unsigned int codepoint) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->charHandler(codepoint);
}

void Window::WindowSizeHandler(GLFWwindow *window, int width, int height) {
  auto userptr = glfwGetWindowUserPointer(window);
  if (userptr)
    static_cast<Window *>(userptr)->windowSizeHandler(width, height);
}

void Window::PointerEventHandler(GLFWwindow *window, const GLFWpointerevent* pointerEvent)
{
	auto userptr = glfwGetWindowUserPointer(window);
	if (userptr)
		static_cast<Window *>(userptr)->pointerEventHandler(pointerEvent);
}

void Window::mouseButtonHandler(int button, int action, int mods) {
  Event ev{EventType::MouseButton};
  ev.mouseButton.button = button;
  ev.mouseButton.action =
      (action == GLFW_PRESS) ? ButtonState::Pressed : ButtonState::Released;
  eventFunc_(*this, ev);
}

void Window::cursorPosHandler(double xpos, double ypos) {
  Event ev{EventType::Cursor};
  ev.cursor.xFloat = xpos;
  ev.cursor.yFloat = ypos;
  ev.cursor.x = (int)xpos;
  ev.cursor.y = (int)ypos;
  eventFunc_(*this, ev);
}

void Window::cursorEnterHandler(int entered) {
  Event ev{entered ? EventType::CursorEnter : EventType::CursorExit};
  eventFunc_(*this, ev);
}

void Window::scrollHandler(double xoffset, double yoffset) {
  Event ev{EventType::MouseScroll};
  ev.scroll.dx = xoffset;
  ev.scroll.dy = yoffset;
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
  eventFunc_(*this, ev);
}

void Window::charHandler(unsigned int codepoint) {
  Event ev{EventType::Text};
  ev.text.codepoint = codepoint;
  eventFunc_(*this, ev);
}

void Window::windowSizeHandler(int width, int height) {
  Event ev{EventType::WindowResize};
  ev.resize.width = width;
  ev.resize.height = height;
  eventFunc_(*this, ev);
}


void Window::pointerEventHandler(const GLFWpointerevent* pointerevent)
{
	EventType type = EventType::PointerMove;
	switch (pointerevent->action) {
	case 1: type = EventType::PointerUp;  break;
	case 2: type = EventType::PointerDown; break;
	case 3: type = EventType::PointerEnter; break;
	case 4: type = EventType::PointerLeave; break;
	case 5: type = EventType::PointerMove; break;
	};
	Event ev{ type };
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

  // set event handlers
  glfwSetWindowSizeCallback(window_, WindowSizeHandler);
  glfwSetCursorEnterCallback(window_, CursorEnterHandler);
  glfwSetMouseButtonCallback(window_, MouseButtonHandler);
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

  // ImGui init
  ImGui_ImplGlfwGL3_Init(window_, false);

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
	while (!glfwWindowShouldClose(window_)) {
		updateStylusInfo();
		// ImGui_ImplGlfwGL3_NewFrame();
		auto framebufferSize = getFramebufferSize();
		ag::gl::resizeDefaultFramebuffer(framebufferSize.x, framebufferSize.y);
		double t = glfwGetTime();
		double dt = t - tlast;
		tlast = t;
		renderFunc_(*this, dt);
		// ImGui::Render();
		ag::gl::endFrame();
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}
}

void Window::updateStylusInfo()
{
#ifdef WIN32
	/*UINT32 entriesCount{ 0 };
	if (!GetPointerPenInfoHistory(1, &entriesCount, nullptr)) {
		AG_DEBUG("GetPointerPenInfoHistory failed");
		fmt::report_windows_error(GetLastError(), "GetPointerPenInfoHistory failed");
		return;
	}
	AG_DEBUG("GetPointerPenInfoHistory entriesCount {}", entriesCount);
	std::vector<POINTER_PEN_INFO> penInfos(entriesCount);
	GetPointerPenInfoHistory(1, &entriesCount, penInfos.data());
	for (int i = entriesCount-1; i >= 0; --i) {
		auto &penInfo = penInfos[i];
		AG_DEBUG("penFlags {} penMask {} pressure {} pointerInfo.ptPixelLocation {},{} ",
			penInfo.penFlags, 
			penInfo.penMask, 
			penInfo.pressure, 
			penInfo.pointerInfo.ptPixelLocation.x, 
			penInfo.pointerInfo.ptPixelLocation.y);
	}*/
#endif

}

void Window::close() { glfwSetWindowShouldClose(window_, 1); }
}