#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/FileDialog.h>
#include <autograph/Core/Support/ProjectRoot.h>
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>
#include <autograph/Window/WindowGLFW.h>
#include <autograph/Editor/CameraController.h>
#include <autograph/Editor/SceneEditor.h>
#include <autograph/Editor/SceneRenderer.h>
#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Canvas.h"
#include "Scene2D.h"

using namespace ag;
using namespace std::experimental;

/*enum ButtonEvents {
  EV_CLOSE,
  EV_RENDER,
  EV_CLICKED,
  EV_CURSOR,
  EV_DOUBLE_CLICKED,
  EV_HOVER_ENTER,
  EV_HOVER_EXIT
};

template <typename... T> bool any_of(int a, T... as) {
  auto test = [a](bool &r, int b) { r |= a == b };
  bool r = false;
  auto dummy = {0, (args(r, as), 0)...};
  return r;
}


struct Subscription {
	struct Impl {};
	template <typename T> friend class Observable;
	void unsubscribe() { ptr_.reset(); }
	std::shared_ptr<Impl> ptr_ = std::make_shared<Impl>();
};


template <typename T> struct Observable {
  

  // associated generator coroutine
  coroutine_handle<promise_type> genCoro_;
  // list of observers
  struct Observer {
	 std::function<void(T)> fn_;
	 std::weak_ptr<Subscription::Impl> sub_;
  };
  std::vector<Observer> observers_;

  // Default ctor
  Observable() {}
  Observable(coroutine_handle<promise_type> genCoro) : genCoro_{ std::move(genCoro) } {}


};

// Signal a new value
template <typename T>
void signal(Observable<T>, T v) {
	// go through the list of observers and call them, knowing that, in response:
	// - one observer may fire this signal again in its handler
	// - one observer may add another observer to this observable
	//		-> must not invalidate iteration state
	// - one observer may remove an observer (incl. itself) from the list 
	//		-> must not invalidate iteration state
	// - the observer might be a resumed coroutine that owns the given observable
	//	 and that, when resumed, will destroy the observable
	//		-> the other observers should still be called
}

Observable<int> button(Observable<int> events) {
  // pull from event stream
        for
          co_await(auto e : events) {
            switch (e) {
            case EV_RENDER:
              AG_DEBUG("EV_RENDER");
              break;
            case EV_CURSOR:
              AG_DEBUG("EV_CURSOR");
              co_yield EV_CURSOR;
              break;
            }
          }

        auto render = [&]() -> Observable<int> {
          while (true) {
            do {
              next_event = co_await events.next();
            } while (next_event != render);
            co_yield 0;
          }
        };

        auto hitTest = [&]() -> Observable<int> {

        };
}*/

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
  // scene.registerComponentManager(deferredSceneRenderer.getRenderData());

  ////////////////////////////////////////////////////
  // Load plugins
  loadPluginModule("DeferredSceneRenderer");
  auto sceneRenderer =
      createClassInstance<SceneRenderer>("DeferredSceneRenderer");
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
    sceneLoader->loadScene(
        "mesh/blender_chan/Sketchfab_2017_02_12_14_36_10.fbx", scene,
        rootEntity, pool);
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
      gui::enumComboBoxT<SceneRenderer::DebugRenderMode>("debug deferred",
                                                         &mode, names);
      if (sceneRenderer)
        sceneRenderer->renderScene(fbo, sceneObjects, renderables, lights, cam,
                                   mode);
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
