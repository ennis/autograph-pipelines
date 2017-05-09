#pragma once
#include <autograph/Engine/Window.h>
#include <autograph/Gfx/Framebuffer.h>
#include <autograph/Gfx/GfxContext.h>

namespace ag {
// Or 'game loop', or 'global state' or whatever
// Bound to a particular render window
class AG_ENGINE_API Engine {
public:
  Engine(Window &window);
  ~Engine();
  // receive events from the specified window
  void setMainWindow(Window *window);
  void setEventCallback(std::function<void(const Event &, Window *)> callback) {
    eventCallback_ = std::move(callback);
  }
  // manually send an event to the engine
  void processEvent(const Event &, Window *w);
  template <typename Callback> void renderFrame(Callback callback) {
    beginFrame();
    if (mainWindow_) {
      auto &fbo = beginWindow(*mainWindow_);
      callback(dt_, fbo);
      endWindow(*mainWindow_, fbo);
    }
    endFrame();
  }

private:
  std::unique_ptr<GfxContext> gfxContext_;
  double dt_ = 0.0;
  double lastTime_ = 0.0;
  Window *mainWindow_ = nullptr;
  bool enableProfiler_ = false;
  bool enableDebugOverlay_ = false;
  std::function<void(const Event &, Window *)> eventCallback_;
  void initialize();
  void beginFrame();
  void endFrame();
  Framebuffer& beginWindow(Window &w);
  void endWindow(Window &w, Framebuffer &fbo);
  bool processEventHooks(const Event& ev, Window* w);
};

// get the engine instance
// AG_ENGINE_API Engine &getEngine();
}
