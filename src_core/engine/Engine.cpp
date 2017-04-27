#include <autograph/engine/Engine.h>

namespace ag 
{
	
  void Engine::bindWindow(Window* window) {
	/*mainWindow_ = window;
	if (mainWindow_ != nullptr) {
		AG_DEBUG("Engine: setting headless mode");
	} else {

	}*/
	AG_DEBUG("Engine: binding window inputs");
	window->setEventCallback([this](const Event& event, Window* w) {
		onEvent(event, w);
	});
  }

  void Engine::processEvent(const Event &, Window* w)
  {
  	
  }

  void Engine::step(double dt) {

  }
}