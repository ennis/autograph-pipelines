#pragma once 
#include <autograph/engine/Application.h>
#include <autograph/engine/Input.h>

struct GLFWwindow;

namespace ag
{
	//////////////////////////////////////////////
	enum class EventType {
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
		WindowResize
	};

	//////////////////////////////////////////////
	struct CursorEnterEvent {};
	struct CursorExitEvent {};

	//////////////////////////////////////////////
	struct WindowResizeEvent
	{
		int width;
		int height;
	};

	//////////////////////////////////////////////
	struct MouseButtonEvent
	{
		int button;
		ButtonState action;
	};

	//////////////////////////////////////////////
	struct CursorEvent
	{
		// in client units (pixels)
		int x;
		int y;
		double xFloat;
		double yFloat;
	};

	//////////////////////////////////////////////
	struct MouseMoveEvent 
	{
		double dx;
		double dy;
	};

	//////////////////////////////////////////////
	struct MouseScrollEvent
	{
		double dx;
		double dy;
	};

	//////////////////////////////////////////////
	struct KeyEvent
	{
		int key;
		int scancode;
		KeyState action;
	};

	//////////////////////////////////////////////
	struct TextEvent 
	{
		char32_t codepoint;
	};

	//////////////////////////////////////////////
	struct StylusProximityEvent
	{
		// TODO
	};

	//////////////////////////////////////////////
	struct StylusPropertiesEvent
	{
		double x;
		double y;
		double pressure;
		double tilt;
	};


	// Window events
	// ripoff of SFML event class
	struct Event {
		Event(EventType ty) : type{ ty } {}
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
		};
	};

	// A rendering window
	class AG_API Window
	{
	public:
		Window(int w, int h, const char* title);
		~Window();

		ivec2 getFramebufferSize();
		ivec2 getWindowSize();
		ivec2 getCursorPosition();

		void onRender(std::function<void(Window&)> renderFunc) {
			renderFunc_ = std::move(renderFunc);
		}
		void onEvent(std::function<void(Window&, const Event&)> eventFunc) {
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

		static void MouseButtonHandler(GLFWwindow *window, int button, int action, int mods);
		static void CursorPosHandler(GLFWwindow *window, double xpos, double ypos);
		static void CursorEnterHandler(GLFWwindow *window, int entered);
		static void ScrollHandler(GLFWwindow *window, double xoffset, double yoffset);
		static void KeyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
		static void CharHandler(GLFWwindow *window, unsigned int codepoint);
		static void WindowSizeHandler(GLFWwindow *window, int width, int height);

		std::function<void(Window&)> renderFunc_; 
		std::function<void(Window&, const Event&)> eventFunc_;
		GLFWwindow* window_;
	};
}