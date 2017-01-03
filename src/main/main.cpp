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

#include <autograph/engine/Shader.h>
#include <autograph/engine/Mesh.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/CameraControl.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/Arcball.h>
#include <autograph/engine/ImageUtils.h>
#include <autograph/engine/Window.h>
#include <autograph/engine/ScriptContext.h>
#include <autograph/engine/RenderTarget.h>
#include "SceneRenderer.h"

using namespace ag;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
std::string pathCombine(std::string a, std::string b) {
	std::experimental::filesystem::path p{ std::move(a) };
	p /= std::move(b);
	return p.string();
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void drawMesh(Mesh &mesh, DrawPass *drawPass, sol::table args) {
	AG_DEBUG("drawMesh: mesh={}, drawPass={}", (void *)&mesh, (void *)drawPass);
	args.for_each([](sol::object key, sol::object value) {
		AG_DEBUG("key={}, value={}", key.as<std::string>(),
			value.as<std::string>());
	});
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
	using namespace ag;
	AddResourceDirectory(getActualPath("resources/"));

	Window w{ 640, 480, "???" };

	ScriptContext lua;

	// bindings
	lua.new_usertype<DeferredSceneRenderer>(
		"DeferredSceneRenderer", sol::call_constructor,
		sol::constructors<sol::types<>>(), "renderScene",
		&DeferredSceneRenderer::renderScene);
	lua.new_usertype<DeferredSceneRenderer::GBuffer>(
		"DeferredGBuffer", sol::call_constructor,
		sol::constructors<sol::types<int, int>>(), "release",
		&DeferredSceneRenderer::GBuffer::release, "diffuseColor",
		sol::property(&DeferredSceneRenderer::GBuffer::getDiffuseTarget), "depth",
		sol::property(&DeferredSceneRenderer::GBuffer::getDepthTarget));
	lua.new_usertype<WireframeOverlayRenderer>(
		"WireframeOverlayRenderer", sol::call_constructor,
		sol::constructors<sol::types<>>(), "renderSceneObject",
		&WireframeOverlayRenderer::renderSceneObject);

	bool initOk = false;
	bool reloadOk = false;
	bool lastOnRenderFailed = false; 
	
	auto tex = loadTexture("img/cavestory/PrtMimi");

	// from Lua:
	// ag.getTexture('img/cavestory/PrtMimi', ImageFormat.R8G8B8A8_SRGB)

	// call init
	auto init = [&]() {
		try {
			lua["screenWidth"] = 640;
			lua["screenHeight"] = 480;
			lua.script("require 'init'");
			lua.script("init()");
			initOk = true;
		}
		catch (sol::error &e) {
			errorMessage("Error in init script:\n\t{}", e.what());
			errorMessage("Please fix the script and reload (F5 key)");
			initOk = false;
		}
	};
	
	auto reset = [&]() {
		if (initOk)
			lua.script("reset()");
	};
	
	init();

	w.onRender([&](ag::Window& win) {
		auto framebufferSize = win.getFramebufferSize();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, framebufferSize.x, framebufferSize.y);
		glClearColor(60.f / 255.f, 60.f / 255.f, 168.f / 255.f, 1.0f);
		glClearDepth(1.0);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		if (!lastOnRenderFailed) {
			try {
				lua.script("render()");
			}
			catch (sol::error &e) {
				errorMessage("Error running render():\n\t{}", e.what());
				errorMessage("Please fix the script and reload (F5 key)");
				lastOnRenderFailed = true;
			}
		}
	});

	/*w.onResize([&](ag::Window& win, int w, int h) {
		AG_DEBUG("resize {} {}", w, h);
		gl::resizeDefaultFramebuffer(w, h);
		lua["screenWidth"] = w;
		lua["screenHeight"] = h;
		lua["resize"](w, h);
	});*/

	w.onEvent([&](ag::Window& win, const ag::Event& ev) {
		/*if (auto keyEvent = ev->as<ag::KeyEvent>()) {
			if (keyEvent->code == GLFW_KEY_F5 &&
				keyEvent->state == ag::KeyState::Pressed) {
				if (!initOk) {
					init();
				}
				else {
					reload();
				}
			}
			else if (keyEvent->code == GLFW_KEY_BACKSPACE && keyEvent->state == ag::KeyState::Pressed) {
				reset();
			}
			sol::optional<sol::function> keyboardInputFn = lua["keyboardInput"];
			if (keyboardInputFn)
				(*keyboardInputFn)(keyEvent->code, keyEvent->state);
		}
		else if (auto curEv = ev->as<ag::CursorEvent>()) {
			sol::optional<sol::function> mouseInputFn = lua["mouseInput"];
			if (mouseInputFn)
				(*mouseInputFn)(curEv->position.x, curEv->position.y);
		}
		else if (auto mouseEv = ev->as<ag::MouseButtonEvent>()) {
			if (mouseEv->state == ButtonState::Pressed) {
				auto curPos = GetCursorPosition();
			}
			else {
			}
		}*/
	});

	w.show();

	return 0;
}
