#include <autograph/Engine/All.h>
#include <autograph/Window/WindowGLFW.h>
#include <autograph/editor/SceneRenderer.h>
#include <autograph/editor/SceneEditor.h>
#include <autograph/editor/CameraController.h>
#include <autograph/Gfx/All.h>
#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/FileDialog.h>
#include <autograph/Core/Support/ProjectRoot.h>
#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Canvas.h"
#include "Scene2D.h"

using namespace ag;


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  using namespace ag;
  ResourceManager::addResourceDirectory(getActualPath("resources/"));

  ////////////////////////////////////////////////////
  // Global vars
  WindowGLFW w{640, 480, "???"};
  Engine engine{w};

  ScriptContext lua;
  Canvas canvas{1024, 1024};
  CanvasRenderer canvasRenderer;
  EntityManager entityManager;
  SceneObjectComponents sceneObjects;
  LightComponents lights;
  RenderableComponents renderables;
  ResourcePool pool;
  Scene scene{entityManager};
  scene.registerComponentManager(sceneObjects);
  scene.registerComponentManager(lights);
  scene.registerComponentManager(renderables);
  //scene.registerComponentManager(deferredSceneRenderer.getRenderData());

  ////////////////////////////////////////////////////
  // Load plugins
  loadPluginModule("DeferredSceneRenderer");
  auto sceneRenderer = createClassInstance<SceneRenderer>("DeferredSceneRenderer");
  assert(sceneRenderer);
  loadPluginModule("SceneEditor");
  auto sceneEditor = createClassInstance<SceneEditor>("SceneEditor");
  assert(sceneEditor);
  loadPluginModule("SceneLoader");
  auto sceneLoader = createClassInstance<SceneLoader>("AssimpSceneLoader");
  loadPluginModule("CameraControl");
  auto camController =
      createClassInstance<CameraController>("CameraController");

  ////////////////////////////////////////////////////
  // Scene setup
  ID rootEntity = 0;
  if (sceneLoader) 
	  sceneLoader->loadScene("mesh/blender_chan/Sketchfab_2017_02_12_14_36_10.fbx", scene, rootEntity, pool);
  SceneObject *rootSceneObj = sceneObjects.get(rootEntity);
  if (rootSceneObj) {
    rootSceneObj->localTransform.scaling = vec3{1.0f};
    rootSceneObj->calculateWorldTransform();
    rootSceneObj->calculateWorldBounds();
    if (camController)
      camController->focusOnObject(scene, *rootSceneObj);
  }
  ID selectedItemID = 0;

  ////////////////////////////////////////////////////
  // Main frame callback
  auto frameCallback = [&](double dt, Framebuffer& fbo) {
	//---------------------------
	// Camera update
	auto screenSizeI = ivec2{ fbo.width(), fbo.height() };
    auto screenSize = vec2{(float)screenSizeI.x, (float)screenSizeI.y};
    Camera cam;
    if (camController) {
      auto cursorPos = w.getCursorPos();
      camController->onCameraGUI(cursorPos.x, cursorPos.y, screenSizeI.x,
                                 screenSizeI.y, cam, scene, selectedItemID);
    }

	//---------------------------
    // Setup & clear framebuffer
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Enable(gl::FRAMEBUFFER_SRGB);
    gl::Viewport(0, 0, screenSizeI.x, screenSizeI.y);
    gl::ClearColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 1.0f);
    gl::ClearDepth(1.0);
    gl::DepthMask(gl::TRUE_);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);

	//---------------------------
	// Background grid
    {
      AG_GPU_PROFILE_SCOPE("Draw grid")
      RenderUtils::drawGrid2D(fbo, screenSize / 2.0f, vec2{25.0f}, 10);
    }

	//---------------------------
	// show scene editor
    {
      AG_PROFILE_SCOPE("Scene editor")
      if (sceneEditor)
        sceneEditor->onSceneEditorGUI(scene, selectedItemID, cam, pool);

      if (sceneLoader) {
        ImGui::Begin("Assimp scene loader");
        if (ImGui::Button("Load scene")) {
          auto res = ag::openFileDialog("", getProjectRootDirectory().c_str());
          if (res) {
            ID rootObj;
            bool result =
                sceneLoader->loadScene(res->c_str(), scene, rootObj, pool);
            if (!result)
              ag::errorMessage("Could not load scene: {}", res->c_str());
          }
        }
        ImGui::End();
      }
    }

    {
      AG_PROFILE_SCOPE("Scene graph update")
      sceneObjects.update();
    }

    {
      AG_GPU_PROFILE_SCOPE("Rendering/Canvas")
      canvasRenderer.renderCanvas(sceneObjects, canvas);
    }

    {
      AG_GPU_PROFILE_SCOPE("Rendering/Deferred")
      static std::pair<const char *, int> names[] = {
          {"None", (int)SceneRenderer::DebugRenderMode::None},
          {"Normals", (int)SceneRenderer::DebugRenderMode::Normals},
          {"ObjectID", (int)SceneRenderer::DebugRenderMode::ObjectID},
          {"Depth", (int)SceneRenderer::DebugRenderMode::Depth},
          {"Positions", (int)SceneRenderer::DebugRenderMode::Positions},
          {"Albedo", (int)SceneRenderer::DebugRenderMode::Albedo},
          {"Velocity", (int)SceneRenderer::DebugRenderMode::Velocity},
      };
      static SceneRenderer::DebugRenderMode mode =
		  SceneRenderer::DebugRenderMode::None;
      gui::enumComboBoxT<SceneRenderer::DebugRenderMode>(
          "debug deferred", &mode, names);
      if (sceneRenderer)
       sceneRenderer->renderScene(fbo, sceneObjects, renderables, lights,
                                        cam, mode);
    }
  };

  ////////////////////////////////////////////////////
  // window events
  auto eventCallback = [&](const ag::Event &ev, ag::Window *win) {
    if (ev.type == EventType::WindowResize) {
      if (sceneRenderer)
		sceneRenderer->resize(ev.resize.width, ev.resize.height);
    }
  };

  ////////////////////////////////////////////////////
  // GLFW main loop
  engine.setEventCallback(eventCallback);
  while (!glfwWindowShouldClose(w.getGLFWwindow())) {
	  // Prepare for frame rendering 
	  engine.renderFrame(frameCallback);
	  glfwPollEvents();
  }

  return 0;
}
