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

void centerCameraOnObject(CameraControl &camCtl, const SceneObject &obj) {
  auto aabb = obj.getApproximateWorldBounds();
  auto size = std::max({aabb.width(), aabb.height(), aabb.depth()});
  auto cx = (aabb.xmax + aabb.xmin) / 2.f;
  auto cy = (aabb.ymax + aabb.ymin) / 2.f;
  auto cz = (aabb.zmax + aabb.zmin) / 2.f;
  camCtl.setNearFarPlanes(0.01f, 2.0f*cz);
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
  Canvas canvas{1024, 1024};
  CanvasRenderer canvasRenderer;
  CameraControl camCtl;
  EntityManager entityManager;
  Scene scene;
  LightScene lightScene;
  RenderableScene renderableScene;
  ResourcePool pool;
  DeferredSceneRenderer::GBuffer deferredGBuffers{ 640, 480 };
  DeferredSceneRenderer deferredSceneRenderer;

  ID rootEntity = loadScene("mesh/youmu/youmu", entityManager,  scene, renderableScene, lightScene, pool);
  SceneObject *rootSceneObj = scene.get(rootEntity);
  if (rootSceneObj) {
	rootSceneObj->localTransform.scaling = vec3{1.0f};
	rootSceneObj->calculateWorldTransform();
	rootSceneObj->calculateWorldBounds();
    centerCameraOnObject(camCtl, *rootSceneObj);
  }

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
    camCtl.setAspectRatio(screenSize.x / screenSize.y);
    Camera cam = camCtl.getCamera();
    // show scene editor
    gui::sceneEditor(cam, entityManager, scene, renderableScene, pool, rootEntity);
	rootSceneObj->calculateWorldTransform();
	rootSceneObj->calculateWorldBounds();
    canvasRenderer.renderCanvas(scene, canvas);
	deferredSceneRenderer.renderScene(deferredGBuffers, scene, renderableScene, cam);

    for (auto &idSceneObj : scene.getObjects()) {
		auto& sceneObj = idSceneObj.second;
      if (sceneObj.mesh) {
        renderUtils.drawMesh(gl::getDefaultFramebuffer(), cam, *sceneObj.mesh,
			sceneObj.worldTransform);
        renderUtils.drawWireMesh(gl::getDefaultFramebuffer(), cam,
                                 *sceneObj.mesh, sceneObj.worldTransform);
      }
    }
  });

  w.onEvent([&](ag::Window& win, const ag::Event& ev) {
	  if (ev.type == EventType::WindowResize) {
		  deferredGBuffers = DeferredSceneRenderer::GBuffer{ ev.resize.width, ev.resize.height };
	  }
  });

  w.show();
  return 0;
}
