// Qt stuff
#include <experimental/filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <autograph/engine/Application.h>
#include <autograph/engine/Input.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>

#include "Bindings.h"
#include "Effect.h"
#include "Mesh.h"
#include "Scene.h"
#include "SceneRenderer.h"

using namespace ag;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
std::string pathCombine(std::string a, std::string b) {
  std::experimental::filesystem::path p{std::move(a)};
  p /= std::move(b);
  return p.string();
}

sol::state *gLuaState = nullptr;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void drawMesh(Mesh &mesh, DrawPass *drawPass, sol::table args) {
  AG_DEBUG("drawMesh: mesh={}, drawPass={}", (void *)&mesh, (void *)drawPass);
  args.for_each([](sol::object key, sol::object value) {
    AG_DEBUG("key={}, value={}", key.as<std::string>(),
             value.as<std::string>());
  });
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void addPackagePath(sol::state &state, const char *path) {
  std::string package_path = state["package"]["path"];
  state["package"]["path"] =
      package_path + (!package_path.empty() ? ";" : "") + path;
}

void LoadImguiBindings(lua_State* s);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
class EditorApplication : public ag::Application {
public:
  EditorApplication() : ag::Application{ag::ivec2{640, 480}} 
  {
	  reloadPipeline();
  }

  void onInputEvent(ag::InputEvent &ev) override {
    if (auto keyEvent = ev.as<ag::KeyEvent>()) {
      if (keyEvent->code == GLFW_KEY_F5 &&
          keyEvent->state == ag::KeyState::Pressed) {
        reloadPipeline();
      }
    }
  }

  void reloadPipeline() {
    AG_DEBUG("==============================================");
    AG_DEBUG("Reloading pipelines");
    scene = std::make_unique<Scene>();
    sceneRenderer = std::make_unique<SceneRenderer>();

    auto &lua = *gLuaState;
    lua.require("__bindings",
                sol::c_call<decltype(&openLuaBindings), &openLuaBindings>);

    try {
      lua.script_file(getActualPath("resources/scripts/init.lua"));
      lua.script("init()");
	  lastOnRenderFailed = false;
    } catch (sol::error &e) {
      errorMessage("Error loading init script:\n\t{}", e.what());
    }
  }

  void render() override {
    auto framebufferSize = getFramebufferSize();
    glViewport(0, 0, framebufferSize.x, framebufferSize.y);
    glClearColor(60.f / 255.f, 60.f / 255.f, 168.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	auto &lua = *gLuaState;
	lua["framebufferWidth"] = framebufferSize.x;
	lua["framebufferHeight"] = framebufferSize.y;

	if (!lastOnRenderFailed) {
		try {
			lua.script("onRender()");
		}
		catch (sol::error& e) {
			errorMessage("Error running onRender:\n\t{}", e.what());
			errorMessage("Please fix the script and reload (F5 key)");
			lastOnRenderFailed = true;
		}
	}
  }

  void resize(ivec2 size) override {
    AG_DEBUG("resize {} {}", size.x, size.y);
    gl::resizeDefaultFramebuffer(size);
  }

private:
	bool lastOnRenderFailed{ false };
  std::unique_ptr<Scene> scene;
  std::unique_ptr<SceneRenderer> sceneRenderer;
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  // initialize Lua VM
  sol::state luaState;
  luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::ffi,
                          sol::lib::jit, sol::lib::string, sol::lib::io,
                          sol::lib::math);
  gLuaState = &luaState; 
  LoadImguiBindings(luaState.lua_state());
  addPackagePath(luaState, getActualPath("resources/scripts/?.lua").c_str());
  EditorApplication ea{};
  ea.run();
  return 0;
}
