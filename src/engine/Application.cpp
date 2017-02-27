#include <autograph/engine/Application.h>
#include <autograph/engine/Input.h>
#include <autograph/engine/ResourceManager.h>
#include <autograph/gl/Device.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>

#include <GrContext.h>
#include <SkCanvas.h>
#include <SkSurface.h>
#include <stdexcept>

#include "Bindings.h"

namespace ag {

struct ApplicationState {
  //ResourceManager resourceManager;
};

static std::unique_ptr<ApplicationState> gApplicationState{nullptr};

static void ensureApplicationStateInitialized() {
  if (!gApplicationState) {
    gApplicationState = std::make_unique<ApplicationState>();
  }
}

/*void Application::initialize(ivec2 initSize) {
  AG_DEBUG("====== Initializing =====================================");
  // Initialize the library

  // subscribe to input events
  ag::onInputEvent.subscribe(impl->subscription,
                                [this](auto ev) { this->onInputEvent(*ev); });

  // init Lua context
  AddResourceDirectory(getActualPath("resources").c_str());
  // load default bindings
  auto framebufferSize = getFramebufferSize();
  impl->lua["g_framebufferWidth"] = framebufferSize.x;
  impl->lua["g_framebufferHeight"] = framebufferSize.y;
  AG_DEBUG("====== Done =============================================");
}*/
}
