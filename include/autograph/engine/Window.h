#pragma once 
#include <autograph/engine/Application.h>
#include <autograph/engine/Input.h>

struct GLFWwindow;

namespace ag
{
	//////////////////////////////////////////////
	enum class EventType {
		MouseButton,
		MouseMove,
		Cursor,
		MouseScroll,
		Key,
		Text,
		StylusProximity,
		StylusProperties,
		WindowResize
	};

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
		ButtonState state;
	};

	//////////////////////////////////////////////
	struct CursorEvent
	{
		// in client units (pixels)
		int x;
		int y;
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
		uint32_t code;
		KeyState state;
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
		void onWindowSizeChanged(int width, int height);

	private:
		void mouseButtonHandler(int button, int action, int mods);
		void cursorPosHandler(double xpos, double ypos);
		void cursorEnterHandler(int entered);
		void scrollHandler(double xoffset, double yoffset);
		void keyHandler(int key, int scancode, int action, int mods);
		void charHandler(unsigned int codepoint);

		static void MouseButtonHandler(GLFWwindow *window, int button, int action, int mods);
		static void CursorPosHandler(GLFWwindow *window, double xpos, double ypos);
		static void CursorEnterHandler(GLFWwindow *window, int entered);
		static void ScrollHandler(GLFWwindow *window, double xoffset, double yoffset);
		static void KeyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
		static void CharHandler(GLFWwindow *window, unsigned int codepoint);

		std::function<void(Window&)> renderFunc_; 
		std::function<void(Window&, const Event&)> eventFunc_;
		GLFWwindow* window_;
	};
}