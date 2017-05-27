#include <OpenImageIO/imagecache.h>
#include <OpenImageIO/imageio.h>
#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/FileDialog.h>
#include <autograph/Core/Support/ProjectRoot.h>
#include <autograph/Editor/CameraController.h>
#include <autograph/Editor/SceneEditor.h>
#include <autograph/Editor/SceneRenderer.h>
#include <autograph/Engine/All.h>
#include <autograph/Engine/FrameGraph.h>
#include <autograph/Gfx/All.h>
#include <autograph/Window/WindowGLFW.h>
#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Canvas.h"
#include "Renderer.h"
#include "Scene2D.h"

using namespace ag;
using namespace std::experimental;

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
  Scene scene{entityManager};
  Cache &cache = Cache::getDefault();
  scene.registerComponentManager(sceneObjects);
  scene.registerComponentManager(lights);
  scene.registerComponentManager(renderables);
  // scene.registerComponentManager(deferredSceneRenderer.getRenderData());
  GeometryPass geometryPass;
  TemporalAntiAliasingPass TAAPass;

  // Sparse texture test
  Texture sparseTex = Texture::create2D(
      ImageFormat::R8G8B8A8_SRGB, 16384, 16384, Texture::MipMaps{1},
      Texture::Samples{0}, Texture::Options::SparseStorage);
  // get sparse texture size
  ivec3 texTileSize = sparseTex.getTileSize();
  AG_DEBUG("texture tile size = {}", texTileSize);

  ////////////////////////////////////////////////////
  // Load plugins
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
    sceneLoader->loadScene(
        "mesh/blender_chan/Sketchfab_2017_02_12_14_36_10.fbx", scene,
        rootEntity, Cache::getDefault());
  SceneObject *rootSceneObj = sceneObjects.get(rootEntity);
  if (rootSceneObj) {
    rootSceneObj->localTransform.scaling = vec3{1.0f};
    rootSceneObj->calculateWorldTransform();
    rootSceneObj->calculateWorldBounds();
    if (camController)
      camController->focusOnObject(scene, *rootSceneObj);
  }
  ID selectedItemID = 0;
  int deferredDebugMode = 0;
  Camera prevCam;
  int AACurSample = 0;

  ////////////////////////////////////////////////////
  // Main frame callback
  auto frameCallback = [&](double dt, Framebuffer &fbo) {
    //---------------------------
    // Camera update
    auto screenSizeI = ivec2{fbo.width(), fbo.height()};
    auto screenSize = vec2{(float)screenSizeI.x, (float)screenSizeI.y};
    Camera cam;
    if (camController) {
      auto cursorPos = w.getCursorPos();
      camController->onCameraGUI(cursorPos.x, cursorPos.y, screenSizeI.x,
                                 screenSizeI.y, cam, scene, selectedItemID);
    }
	if (AACurSample == 0)
		prevCam = cam;

    //---------------------------
    // Setup the rendering pipeline
    auto camdata = makeCameraFrameData(cam, prevCam, screenSizeI.x, screenSizeI.y, 0);
    FrameGraph fg;
    // init gbuffers
    auto gbuffers = initializeGeometryBuffers(fg, screenSizeI.x, screenSizeI.y);
    // render scene with geometry pass
    gbuffers = geometryPass.addPass(fg, gbuffers, sceneObjects, renderables,
                                    lights, cam, camdata);
    // evaluate deferred shading
    auto result_deferred = addDeferredEvalPass(
        fg, screenSizeI.x, screenSizeI.y, camdata, gbuffers, deferredDebugMode);
    // Temporal AA pass
    auto result_taa = TAAPass.addPass(fg, result_deferred, gbuffers.velocity,
                                      gbuffers.depth, camdata);
    // compile the pipeline
    fg.compile();
	fg.dumpGraph("machin.dot");

    //---------------------------
    // Setup & clear framebuffer
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Enable(gl::FRAMEBUFFER_SRGB);
    gl::Viewport(0, 0, screenSizeI.x, screenSizeI.y);
    gl::ClearColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 1.0f);
    gl::ClearDepth(1.0);
    gl::DepthMask(gl::TRUE_);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT |
              gl::STENCIL_BUFFER_BIT);

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
        sceneEditor->onSceneEditorGUI(scene, selectedItemID, cam, cache);

      if (sceneLoader) {
        ImGui::Begin("Assimp scene loader");
        if (ImGui::Button("Load scene")) {
          auto res = ag::openFileDialog("", getProjectRootDirectory().c_str());
          if (res) {
            ID rootObj;
            bool result =
                sceneLoader->loadScene(res->c_str(), scene, rootObj, cache);
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

    /*  static std::pair<const char *, int> names[] = {
          {"None", (int)SceneRenderer::DebugRenderMode::None},
          {"Normals", (int)SceneRenderer::DebugRenderMode::Normals},
          {"ObjectID", (int)SceneRenderer::DebugRenderMode::ObjectID},
          {"Depth", (int)SceneRenderer::DebugRenderMode::Depth},
          {"Positions", (int)SceneRenderer::DebugRenderMode::Positions},
          {"Albedo", (int)SceneRenderer::DebugRenderMode::Albedo},
          {"Velocity", (int)SceneRenderer::DebugRenderMode::Velocity},
      };*/

    ImGui::SliderInt("Deferred debug", &deferredDebugMode, 0, 6);

	prevCam = cam;

  };

  ////////////////////////////////////////////////////
  // window events
  auto eventCallback = [&](const ag::Event &ev, ag::Window *win) {
    if (ev.type == EventType::WindowResize) {
      // if (sceneRenderer)
      // sceneRenderer->resize(ev.resize.width, ev.resize.height);
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
