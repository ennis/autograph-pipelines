#include <experimental/filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/packing.hpp>

#include <autograph/engine/All.h>
#include <autograph/gl/All.h>
#include <autograph/support/Debug.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>

#include "Canvas.h"
#include "Scene2D.h"
#include "SceneRenderer.h"

using namespace ag;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
std::string pathCombine(std::string a, std::string b) {
  std::experimental::filesystem::path p{std::move(a)};
  p /= std::move(b);
  return p.string();
}

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

void centerCameraOnObject(CameraControl &camCtl, const SceneObject &obj) {
  auto aabb = obj.getApproximateWorldBoundingBox();
  auto size = std::max({aabb.width(), aabb.height(), aabb.depth()});
  auto cx = (aabb.xmax + aabb.xmin) / 2.f;
  auto cy = (aabb.xmax + aabb.xmin) / 2.f;
  auto cz = (aabb.xmax + aabb.xmin) / 2.f;
  camCtl.setNearFarPlanes(0.01f, 10.0f);
  camCtl.lookAt(cx, cy, cz);
  camCtl.lookDistance(1.5f * size);
  camCtl.setFieldOfView(45.0f);
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  using namespace ag;
  addResourceDirectory(getActualPath("resources/"));

  // Global vars
  Window w{640, 480, "???"};
  ScriptContext lua;
  bool initOk = false;
  bool reloadOk = false;
  bool lastOnRenderFailed = false;
  float currentFps = 0.0f;
  Canvas canvas{1024, 1024};
  CanvasRenderer canvasRenderer;
  CameraControl camCtl;
  ResourcePool pool;

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

  EntityList scene;
  auto ent = SceneUtils::load("meshes/treasure_chest/model", scene, pool);
  auto rootObj = ent->getComponent<SceneObject>();
  rootObj->localTransform.scaling = vec3{0.01f};
  rootObj->updateWorldTransform();
  rootObj->updateBounds();
  centerCameraOnObject(camCtl, *rootObj);

  w.onRender([&](ag::Window &win, double dt) {
    auto screenSizeI = win.getFramebufferSize();
    auto screenSize = vec2{(float)screenSizeI.x, (float)screenSizeI.y};
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(0, 0, screenSizeI.x, screenSizeI.y);
    glClearColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 1.0f);
    glClearDepth(1.0);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    auto &renderUtils = getRenderUtils();
    renderUtils.drawGrid2D(gl::getDefaultFramebuffer(), screenSize / 2.0f,
                           vec2{25.0f}, 10);
    canvasRenderer.renderCanvas(scene, canvas);
    Camera cam = camCtl.getCamera();
    // show scene editor
    gui::sceneEditor(cam, scene, rootObj->id);
    rootObj->updateWorldTransform();
    rootObj->updateBounds();

    for (auto &id_obj : scene.getObjects()) {
      auto sceneObj = id_obj.second.getComponent<SceneObject>();
      if (sceneObj->mesh) {
        renderUtils.drawMesh(gl::getDefaultFramebuffer(), cam, *sceneObj->mesh,
                             sceneObj->worldTransform);
        renderUtils.drawWireMesh(gl::getDefaultFramebuffer(), cam,
                                 *sceneObj->mesh, sceneObj->worldTransform);
      }
    }

  });

  w.onEvent([&](ag::Window &win, const ag::Event &ev) {
    sol::object eventFn = lua["onEvent"];
    if (eventFn.is<sol::function>())
      eventFn.as<sol::function>()(eventToLua(lua, ev));
    if (ev.type == EventType::Key) {
      if (ev.key.action == KeyState::Pressed && ev.key.key == GLFW_KEY_F5) {
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
