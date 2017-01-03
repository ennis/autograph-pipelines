#include <autograph/engine/Window.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <autograph/gl/Device.h>
#include <GLFW/glfw3.h>
#include <gl_core_4_5.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

namespace ag
{
	static void windowSizeChanged(GLFWwindow* window, int width, int height)
	{
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->onWindowSizeChanged(width, height);
	}

	// GLFW event handlers
	void Window::MouseButtonHandler(GLFWwindow *window, int button, int action, int mods) {
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->mouseButtonHandler(button, action, mods);
	}

	void Window::CursorPosHandler(GLFWwindow *window, double xpos, double ypos) {
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->cursorPosHandler(xpos, ypos);
	}

	void Window::CursorEnterHandler(GLFWwindow *window, int entered) {
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->cursorEnterHandler(entered);
	}

	void Window::ScrollHandler(GLFWwindow *window, double xoffset, double yoffset) {
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->scrollHandler(xoffset, yoffset);
	}

	void Window::KeyHandler(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->keyHandler(key, scancode, action, mods);
	}

	void Window::CharHandler(GLFWwindow *window, unsigned int codepoint) {
		auto userptr = glfwGetWindowUserPointer(window);
		if (userptr)
			static_cast<Window*>(userptr)->charHandler(codepoint);
	}


	void Window::mouseButtonHandler(int button, int action, int mods)
	{

	}

	void Window::cursorPosHandler(double xpos, double ypos)
	{

	}

	void Window::cursorEnterHandler(int entered)
	{

	}

	void Window::scrollHandler(double xoffset, double yoffset)
	{

	}

	void Window::keyHandler(int key, int scancode, int action, int mods)
	{

	}

	void Window::charHandler(unsigned int codepoint)
	{

	}

	Window::Window(int w, int h, const char * title)
	{
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

		glfwSetWindowSizeCallback(window_, windowSizeChanged);

		// autograph init
		gl::DeviceConfig devCfg;
		devCfg.init_fb_width = w;
		devCfg.init_fb_height = h;
		devCfg.max_frames_in_flight = 3;
		gl::initialize(devCfg);
		gl::resizeDefaultFramebuffer(w, h);

		// ImGui init
		ImGui_ImplGlfwGL3_Init(window_, false);
	}

	Window::~Window()
	{
		ImGui_ImplGlfwGL3_Shutdown();
		glfwTerminate();
	}

	ivec2 Window::getCursorPosition() {
		double x, y;
		glfwGetCursorPos(window_, &x, &y);
		return ivec2{ (int)x, (int)y };
	}

	ivec2 Window::getFramebufferSize()
	{
		int w, h;
		glfwGetWindowSize(window_, &w, &h);
		return ivec2{ w, h };
	}

	ivec2 Window::getWindowSize() {
		int w, h;
		glfwGetWindowSize(window_, &w, &h);
		return ivec2{ w, h };
	}

	void Window::show()
	{
		while (!glfwWindowShouldClose(window_)) {
			ImGui_ImplGlfwGL3_NewFrame();
			auto framebufferSize = getFramebufferSize();
			renderFunc_(*this);
			ImGui::Render();
			ag::gl::endFrame();
			glfwSwapBuffers(window_);
			glfwPollEvents();
		}
	}

	void Window::close() {
		glfwSetWindowShouldClose(window_, 1);
	}

	void Window::onWindowSizeChanged(int width, int height)
	{
		//resizeFunc_(*this, width, height);
	}
}