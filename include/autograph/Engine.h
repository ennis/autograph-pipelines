#pragma once

namespace ag {
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
  WindowResize,
  PointerEnter,
  PointerDown,
  PointerUp,
  PointerMove,
  PointerLeave
};

//////////////////////////////////////////////
struct CursorEnterEvent {};
struct CursorExitEvent {};

//////////////////////////////////////////////
struct WindowResizeEvent {
  int width;
  int height;
};

//////////////////////////////////////////////
struct MouseButtonEvent {
  int button;
  ButtonState action;
};

//////////////////////////////////////////////
struct CursorEvent {
  // in client units (pixels)
  int x;
  int y;
  double xFloat;
  double yFloat;
};

//////////////////////////////////////////////
struct MouseMoveEvent {
  double dx;
  double dy;
};

//////////////////////////////////////////////
struct MouseScrollEvent {
  double dx;
  double dy;
};

//////////////////////////////////////////////
struct KeyEvent {
  int key;
  int scancode;
  KeyState action;
};

//////////////////////////////////////////////
struct TextEvent {
  char32_t codepoint;
};

//////////////////////////////////////////////
struct StylusProximityEvent {
  // TODO
};

//////////////////////////////////////////////
struct StylusPropertiesEvent {
  double x;
  double y;
  double pressure;
  double tilt;
};

//////////////////////////////////////////////
struct PointerInfo {
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

struct PointerEvent {
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

// == Presentation surface
class Window2 
{
public:
	virtual void setEventCallback(std::function<void(const Event&, Window*)> callback) = 0;
	virtual void swapBuffers() = 0;
	virtual void prepareForRender() = 0;
	virtual gl::Framebuffer& getFramebuffer() = 0;
private:
};

class Engine {
public:
  Engine();

  void bindWindow(Window* window);

  // called once per frame for each window
  void setRenderCallback(std::function<void(double)> callback) {
    renderFunc_ = std::move(callback);
  }

  void setEventCallback(std::function<void(const Event&, Window*)> callback) {
    eventFunc_ = std::move(callback);
  }

  void step(double dt);

private:
  Window* mainWindow_;
  void processEvent(const Event &, Window* w);

  std::function<void(double)> renderFunc_;
  std::function<void(const Event&, Window*)> eventFunc_;
};

{
	// Create the engine (headless mode)
	Engine engine;
	// Create a window and a GL context
	GLFWwindow window{1280,720};
	// Set the engine window
	// The engine will now take inputs from, and render to, the specified window
	engine.setWindow(window);
	
	// 
	// Platform-specific main loop
	glfw::mainLoop();

	// What should be hidden from the user:
	// - passing of application-wide events to the engine
	// - passing of window events to the engine
	// What should not:
	// - let the user decide if a window should be rendered to 
	//		(avoid costly context switching if is it not necessary)


}


}