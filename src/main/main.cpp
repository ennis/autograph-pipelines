#include <experimental/filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
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

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  using namespace ag;
  addResourceDirectory(getActualPath("resources/"));

  ////////////////////////////////////////////////////
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
  DeferredSceneRenderer::GBuffer deferredGBuffers{640, 480};
  DeferredSceneRenderer deferredSceneRenderer;

  ////////////////////////////////////////////////////
  // Scene
  ID rootEntity = loadScene("mesh/sponza/sponza", entityManager, scene,
                            renderableScene, lightScene, pool);
  SceneObject *rootSceneObj = scene.get(rootEntity);
  if (rootSceneObj) {
    rootSceneObj->localTransform.scaling = vec3{1.0f};
    rootSceneObj->calculateWorldTransform();
    rootSceneObj->calculateWorldBounds();
    camCtl.centerOnObject(rootSceneObj->getApproximateWorldBounds());
  }

  ////////////////////////////////////////////////////
  // Render
  w.onRender([&](ag::Window &win, double dt) {
    ////////////////////////////////////////////////////
    auto screenSizeI = win.getFramebufferSize();
    auto screenSize = vec2{(float)screenSizeI.x, (float)screenSizeI.y};
    camCtl.setAspectRatio(screenSize.x / screenSize.y);
    Camera cam = camCtl.getCamera();
    ////////////////////////////////////////////////////
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(0, 0, screenSizeI.x, screenSizeI.y);
    glClearColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 1.0f);
    glClearDepth(1.0);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    auto &renderUtils = getRenderUtils();
    {
      AG_GPU_PROFILE_SCOPE("Draw grid")
      renderUtils.drawGrid2D(gl::getDefaultFramebuffer(), screenSize / 2.0f,
                             vec2{25.0f}, 10);
    }
    // show scene editor
    gui::sceneEditor(cam, entityManager, scene, renderableScene, pool,
                     rootEntity);
    {
      AG_PROFILE_SCOPE("Scene graph update")
      rootSceneObj->calculateWorldTransform();
      rootSceneObj->calculateWorldBounds();
    }

    {
      AG_GPU_PROFILE_SCOPE("Rendering/Canvas")
      canvasRenderer.renderCanvas(scene, canvas);
    }

    {
      AG_GPU_PROFILE_SCOPE("Rendering/Deferred")
      deferredSceneRenderer.renderScene(deferredGBuffers, scene,
                                        renderableScene, cam);
    }

    {
      AG_GPU_PROFILE_SCOPE("Rendering/Debug")
      for (auto &idSceneObj : scene.getObjects()) {
        auto &sceneObj = idSceneObj.second;
        if (sceneObj.mesh) {
          renderUtils.drawMesh(gl::getDefaultFramebuffer(), cam, *sceneObj.mesh,
                               sceneObj.worldTransform);
          renderUtils.drawWireMesh(gl::getDefaultFramebuffer(), cam,
                                   *sceneObj.mesh, sceneObj.worldTransform);
        }
      }
    }
  });

  ////////////////////////////////////////////////////
  // window events
  w.onEvent([&](ag::Window &win, const ag::Event &ev) {
    if (ev.type == EventType::WindowResize) {
      deferredGBuffers =
          DeferredSceneRenderer::GBuffer{ev.resize.width, ev.resize.height};
    }
  });

  w.show();
  return 0;
}
