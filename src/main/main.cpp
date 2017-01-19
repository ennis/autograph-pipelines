#include <experimental/filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <imgui.h>

#include <glm/gtc/packing.hpp>

#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>

#include <autograph/support/Debug.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>

#include "Widget.h"
#include <autograph/engine/Application.h>
#include <autograph/engine/Arcball.h>
#include <autograph/engine/CameraControl.h>
#include <autograph/engine/ImageUtils.h>
#include <autograph/engine/Input.h>
#include <autograph/engine/MathUtils.h>
#include <autograph/engine/Mesh.h>
#include <autograph/engine/Meta.h>
#include <autograph/engine/RenderTarget.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/ScriptContext.h>
#include <autograph/engine/Shader.h>
#include <autograph/engine/Window.h>

#include "Scene2D.h"
#include "SceneRenderer.h"
#include "Canvas.h"

using namespace ag;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
std::string pathCombine(std::string a, std::string b) {
  std::experimental::filesystem::path p{std::move(a)};
  p /= std::move(b);
  return p.string();
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/*void drawMesh(Mesh &mesh, DrawPass *drawPass, sol::table args) {
        AG_DEBUG("drawMesh: mesh={}, drawPass={}", (void *)&mesh, (void
*)drawPass);
        args.for_each([](sol::object key, sol::object value) {
                AG_DEBUG("key={}, value={}", key.as<std::string>(),
                        value.as<std::string>());
        });
}*/

void pointerEventToLua(sol::table &tbl, const PointerInfo &info) {
  tbl["button"] = info.button;
  tbl["buttons"] = info.buttons;
  tbl["id"] = info.id;
  tbl["mask"] = info.mask;
  tbl["type"] = info.type;
  tbl["x"] = info.x;
  tbl["y"] = info.y;
  tbl["pressure"] = info.pressure;
  tbl["tiltX"] = info.tiltX;
  tbl["tiltY"] = info.tiltY;
}

/////////////////////////////////////////////////////////////
sol::table eventToLua(sol::state &L, const ag::Event &ev) {
  // auto table = L.create_table();
  sol::function makeInstance = L["autograph"]["makeClassInstance"];
  switch (ev.type) {
  case ag::EventType::MouseButton: {
    sol::table table = makeInstance("MouseButtonEvent");
    table["type"] = ev.type;
    table["button"] = ev.mouseButton.button;
    table["action"] = ev.mouseButton.action;
    return table;
  }
  case ag::EventType::MouseMove: {
    sol::table table = makeInstance("MouseMoveEvent");
    table["dx"] = ev.mouseMove.dx;
    table["dy"] = ev.mouseMove.dy;
    return table;
  }
  case ag::EventType::Cursor: {
    sol::table table = makeInstance("CursorEvent");
    table["x"] = ev.cursor.x;
    table["y"] = ev.cursor.y;
    return table;
  }
  case ag::EventType::CursorEnter: {
    sol::table table = makeInstance("CursorEnterEvent");
    return table;
  }
  case ag::EventType::CursorExit: {
    sol::table table = makeInstance("CursorExitEvent");
    return table;
  }
  case ag::EventType::MouseScroll: {
    sol::table table = makeInstance("MouseScrollEvent");
    table["dx"] = ev.scroll.dx;
    table["dy"] = ev.scroll.dy;
    return table;
  }
  case ag::EventType::Key: {
    sol::table table = makeInstance("KeyEvent");
    table["key"] = ev.key.key;
    table["action"] = ev.key.action;
    return table;
  }
  case ag::EventType::Text: {
// VS BUG (with char32_t)
#if defined(_MSC_VER)
    std::wstring_convert<std::codecvt_utf8<unsigned int>, unsigned int> cvt;
#else
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
#endif
    sol::table table = makeInstance("TextEvent");
    table["string"] = cvt.to_bytes(ev.text.codepoint);
    return table;
  }
  case ag::EventType::StylusProximity: {
    sol::table table = makeInstance("StylusProximityEvent");
    // TODO
    return table;
  }
  case ag::EventType::StylusProperties: {
    sol::table table = makeInstance("StylusPropertiesEvent");
    // TODO
    return table;
  }
  case ag::EventType::WindowResize: {
    sol::table table = makeInstance("WindowResizeEvent");
    pointerEventToLua(table, ev.pointer.info);
    return table;
  }
  case ag::EventType::PointerDown: {
    sol::table table = makeInstance("PointerDownEvent");
    pointerEventToLua(table, ev.pointer.info);
    return table;
  }
  case ag::EventType::PointerUp: {
    sol::table table = makeInstance("PointerUpEvent");
    pointerEventToLua(table, ev.pointer.info);
    return table;
  }
  case ag::EventType::PointerEnter: {
    sol::table table = makeInstance("PointerEnterEvent");
    pointerEventToLua(table, ev.pointer.info);
    return table;
  }
  case ag::EventType::PointerLeave: {
    sol::table table = makeInstance("PointerLeaveEvent");
    pointerEventToLua(table, ev.pointer.info);
    return table;
  }
  case ag::EventType::PointerMove: {
    sol::table table = makeInstance("PointerMoveEvent");
    pointerEventToLua(table, ev.pointer.info);
    return table;
  }
  default:
    break;
  }
  return sol::table{};
}

template <typename T> bool isSameType(std::type_index ti) {
  return ti == std::type_index{typeid(T)};
}

void imguiInputString(const char *label, std::string &str,
                      size_t buf_size = 100) {
  std::vector<char> strvec{str.begin(), str.end()};
  strvec.resize(buf_size);
  ImGui::InputText("", strvec.data(), strvec.size());
  str.assign(strvec.begin(), strvec.end());
}

const char *getFriendlyName(const meta::Field &f) {
  // if (auto a = f.getAttribute<meta::FriendlyName>()) {
  //	return a->name;
  //}
  // else
  return f.name;
}

const char *getFriendlyName(const meta::Enumerator &e) {
  // if (auto a = e.getAttribute<meta::FriendlyName>()) {
  //	return a->name;
  //}
  // else
  return e.name;
}

void valueDebugGUI(std::type_index ti, void *data) {
  ImGui::PushID(data);
  // GUIs for primitive types
  if (isSameType<float>(ti)) {
    ImGui::SliderFloat("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (isSameType<double>(ti)) {
    // ImGui::SliderD("", reinterpret_cast<float*>(data), 0.0f, 10.0f);
  } else if (isSameType<int>(ti)) {
    ImGui::SliderInt("", reinterpret_cast<int *>(data), 0, 100);
  } else if (isSameType<bool>(ti)) {
    ImGui::Checkbox("", reinterpret_cast<bool *>(data));
  }
  // GUI for std::string
  else if (isSameType<std::string>(ti)) {
    auto &str = *reinterpret_cast<std::string *>(data);
    imguiInputString("", str);
  }
  // GUIs for GLM vector types
  else if (isSameType<vec2>(ti)) {
    ImGui::SliderFloat2("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (isSameType<vec3>(ti)) {
    ImGui::SliderFloat3("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (isSameType<vec4>(ti)) {
    ImGui::SliderFloat4("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (isSameType<ivec2>(ti)) {
    ImGui::SliderInt2("", reinterpret_cast<int *>(data), 0, 100);
  } else if (isSameType<ivec3>(ti)) {
    ImGui::SliderInt3("", reinterpret_cast<int *>(data), 0, 100);
  } else if (isSameType<ivec4>(ti)) {
    ImGui::SliderInt4("", reinterpret_cast<int *>(data), 0, 100);
  }
  // GUIs for reflected enum types
  else if (auto mo0 = meta::typeOf(ti)) {
    if (auto mo = mo0->as<meta::Enum>()) {
      int i = mo->findEnumeratorIndex(data);
      auto items_getter = [](void *data, int idx,
                             const char **out_text) -> bool {
        auto mo = static_cast<const meta::Enum *>(data);
        if (idx >= mo->enumerators.size())
          return false;
        *out_text = getFriendlyName(mo->enumerators[idx]);
        return true;
      };
      ImGui::Combo("", &i, items_getter, const_cast<meta::Enum *>(mo),
                   mo->enumerators.size());
      mo->setValue(data, mo->enumerators[i].value);
    } else if (auto mo = mo0->as<meta::Record>()) {
      if (ImGui::CollapsingHeader(mo->name)) {
        ImGui::Columns(2);
        for (auto &&f : mo->publicFields) {
          ImGui::Text("%s", getFriendlyName(f));
          ImGui::NextColumn();
          ImGui::PushItemWidth(-1.0f);
          valueDebugGUI(f.typeindex, f.getPtr(data));
          ImGui::NextColumn();
        }
        ImGui::Columns(1);
      }
    }
  }
  // GUIs for structs
  else {
    ImGui::TextDisabled("No metaobject");
  }
  ImGui::PopID();
}

template <typename T> void debugValue(T &value) {
  valueDebugGUI(typeid(T), &value);
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  using namespace ag;
  addResourceDirectory(getActualPath("resources/"));

  Window w{640, 480, "???"};

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
  lua.new_usertype<Scene2D>(
      "Scene2D", sol::call_constructor, sol::constructors<sol::types<>>(),
      "loadTilemap", &Scene2D::loadTilemap, "render",
      static_cast<void (Scene2D::*)(gl::Framebuffer &, float, float, float,
                                    float)>(&Scene2D::render));

  bool initOk = false;
  bool reloadOk = false;
  bool lastOnRenderFailed = false;

  Scene scene;
  Canvas canvas{ 1024, 1024 };
  CanvasRenderer canvasRenderer;

  // call init
  auto init = [&]() {
  };

  auto reset = [&]() {
  };

  init();

  w.onRender([&](ag::Window &win, double dt) {
	  auto framebufferSize = win.getFramebufferSize();
	  glBindFramebuffer(GL_FRAMEBUFFER, 0);
	  glEnable(GL_FRAMEBUFFER_SRGB);
	  glViewport(0, 0, framebufferSize.x, framebufferSize.y);
	  glClearColor(60.f / 255.f, 60.f / 255.f, 168.f / 255.f, 1.0f);
	  glClearDepth(1.0);
	  glDepthMask(GL_TRUE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	  canvasRenderer.renderCanvas(scene, canvas);
	  glDisable(GL_STENCIL_TEST);
    // GUI test
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu(ICON_FA_FOLDER " Menu")) {
      ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open...", "Ctrl+O");
      ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Close", "Ctrl+F4");
	  if (ImGui::MenuItem(ICON_FA_REFRESH " Reload shaders")) {
		  canvasRenderer.reloadShaders();
	  }
      ImGui::Separator();
      ImGui::MenuItem(ICON_FA_WINDOW_CLOSE_O " Quit", "Alt+F4");
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

  });

  w.onEvent([&](ag::Window &win, const ag::Event &ev) {
    sol::object eventFn = lua["onEvent"];
    if (eventFn.is<sol::function>())
      eventFn.as<sol::function>()(eventToLua(lua, ev));
    if (ev.type == EventType::Key) {
      if (ev.key.action == KeyState::Pressed && ev.key.key == GLFW_KEY_F5) {
        reset();
      } else if (ev.key.action == KeyState::Pressed &&
                 ev.key.key == GLFW_KEY_F4) {
      }
    } else if (ev.type == EventType::PointerDown) {
    } else if (ev.type == EventType::PointerUp) {
    } else if (ev.type == EventType::PointerMove) {
    }
    /*AG_DEBUG("pointer({}, type {}, mask{}): button {} pos {},{} pressure {}",
             ev.pointer.info.id, ev.pointer.info.type, ev.pointer.info.mask,
             ev.pointer.info.button, ev.pointer.info.x, ev.pointer.info.y,
             ev.pointer.info.pressure);*/
  });

  w.show();

  return 0;
}
