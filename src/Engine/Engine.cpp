#include "imgui/imgui_impl_gl3.h"
#include <autograph/Engine/DebugOverlay.h>
#include <autograph/Engine/Engine.h>
#include <autograph/Engine/ImGuiUtils.h>
#include <autograph/Engine/Plugin.h>
#include <autograph/Engine/Profiler.h>
#include <autograph/Engine/Shader.h>
#include <chrono>

namespace ag {

Engine::Engine(Window &w) : mainWindow_{nullptr} {
  setMainWindow(&w);
  initialize();
}

Engine::~Engine() {
  if (mainWindow_) {
    mainWindow_->unsetEventCallback();
  }
}

void Engine::setMainWindow(Window *window) {
  if (mainWindow_) {
    mainWindow_->unsetEventCallback();
  }
  mainWindow_ = window;
  if (mainWindow_) {
    window->setEventCallback(
        [this](const Event &event, Window *w) { processEvent(event, w); });
  }
}

// Returns false if the event should not be further processed, true otherwise
bool Engine::processEventHooks(const Event &ev, Window *w) {
  if (ev.type == EventType::Key) {
    const int key = ev.key.key;
    const int mods = ev.key.mods;
    const KeyAction action = ev.key.action;
    // Debug hooks
    if (mods == KEY_MOD_CONTROL && key == KEY_F12 &&
        action == KeyAction::Press) {
      // Toggle debug overlay
      enableDebugOverlay_ = !enableDebugOverlay_;
	  return false;
    }
    if (mods == (KEY_MOD_CONTROL | KEY_MOD_SHIFT) && key == KEY_F12 &&
        action == KeyAction::Press) {
      // capture next frame
      gfxContext_->setNextFrameCapture();
	  return false;
    }
    if (mods == (KEY_MOD_CONTROL) && key == KEY_F8 &&
        action == KeyAction::Press) {
      // toggle profiler
      enableProfiler_ = !enableProfiler_;
      AG_DEBUG("Profiler {}", enableProfiler_ ? "ON" : "OFF");
	  return false;
    }
    if (mods == (KEY_MOD_CONTROL) && key == KEY_F5 &&
        action == KeyAction::Press) {
      // reload all shaders
      getPipelineStateCache().reloadAll();
	  return false;
    }
  }
  return true;
}

void Engine::processEvent(const Event &event, Window *w) {
  if (processEventHooks(event, w)) {
	// forward events to ImGui (should take predecence)
	ImGui_Impl_ProcessEvent(event);
	// forward to user callback
    if (eventCallback_)
      eventCallback_(event, w);
  }
}

void Engine::initialize() {
  //----------------------------------
  // Low-level graphics init
  GfxConfig cfg;
  cfg.maxFramesInFlight = 3;
  cfg.defaultUploadBufferSize = 3 * 1024 * 1024;
  gfxContext_ = std::make_unique<GfxContext>(cfg);
  ImGui_Impl_Init();
}

static void setupDefaultGLState() {
  gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
  gl::Disable(gl::STENCIL_TEST);
  gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
}

void Engine::beginFrame() {
  double t =
      std::chrono::duration<double>{
          std::chrono::high_resolution_clock::now().time_since_epoch()}
          .count();
  dt_ = t - lastTime_;
  lastTime_ = t;
  if (enableProfiler_)
    Profiler::beginFrame();
}

Framebuffer &Engine::beginWindow(Window &w) {
  //--------------------------------
  // window setup
  w.prepareForRender();
  auto fbSize = mainWindow_->getFramebufferSize();
  gfxContext_->resizeRenderWindow(fbSize.x, fbSize.y);
  gfxContext_->beginFrame();
  ImGui_Impl_NewFrame(w, lastTime_);
  setupDefaultGLState();
  gui::beginFrame();
  return gfxContext_->getDefaultFramebuffer();
}

void Engine::endWindow(Window &w, Framebuffer &fbo) {
  //----------------------------------
  // profiler GUI
  gl::BindFramebuffer(gl::FRAMEBUFFER, fbo.object());
  if (enableProfiler_)
    Profiler::showGui();
  if (enableDebugOverlay_) {
    AG_GPU_PROFILE_SCOPE("Debug overlay")
    drawDebugOverlay(dt_);
  }
  gui::endFrame();
  {
    AG_GPU_PROFILE_SCOPE("IMGUI")
    ImGui::Render();
  }
  {
    AG_GPU_PROFILE_SCOPE("Present")
    w.swapBuffers();
  }
}

void Engine::endFrame() {

  //----------------------------------
  // Graphics end frame
  gfxContext_->endFrame();
  //----------------------------------
  // Plugins
  // reload plugins if necessary
  watchForPluginModuleChanges();
  //----------------------------------
  // Profiling
  // end profiling frame
  Profiler::endFrame();
}

} // namespace ag