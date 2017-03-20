#include <autograph/engine/All.h>
#include <autograph/gl/All.h>
#include <autograph/support/Debug.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>
#include <experimental/filesystem>
#include <fmt/format.h>
#include <glm/gtc/packing.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Canvas.h"
#include "Scene2D.h"
#include "SceneRenderer.h"
#include <autograph/engine/imgui.h>

using namespace ag;


class ISceneRenderer
{
public:
	enum class DebugRenderMode {
		None = 0,
		Normals = 1,
		ObjectID = 2,
		Depth = 3,
		Positions = 4,
		Albedo = 5,
		Velocity = 6,
	};

	virtual void resize(int width, int height) = 0;
	virtual void renderScene(gl::Framebuffer &target, SceneObjectComponents &sceneObjects,
		RenderableComponents &renderables,
		LightComponents& lights,
		const Camera &camera, DebugRenderMode debugRenderMode = DebugRenderMode::None) = 0;

private:
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  using namespace ag;
  ResourceManager::addResourceDirectory(getActualPath("resources/"));

  ////////////////////////////////////////////////////
  // Global vars
  Window w{640, 480, "???"};
  ScriptContext lua;
  Canvas canvas{1024, 1024};
  CanvasRenderer canvasRenderer;
  CameraControl camCtl;
  EntityManager entityManager;
  SceneObjectComponents sceneObjects;
  LightComponents lights;
  RenderableComponents renderables;
  ResourcePool pool;
  DeferredSceneRenderer deferredSceneRenderer{640, 480};
  Scene scene{ entityManager };
  scene.registerComponentManager(sceneObjects);
  scene.registerComponentManager(lights);
  scene.registerComponentManager(renderables);
  scene.registerComponentManager(deferredSceneRenderer.getRenderData());
  
  loadPluginModule("DeferredSceneRenderer");
  auto sceneRenderer = createClassInstance<Extension>("DeferredSceneRenderer");
  assert(!sceneRenderer);
  loadPluginModule("SceneEditor");
  auto sceneEditor = createClassInstance<ISceneEditor>("SceneEditor");
  assert(!sceneEditor);

  ////////////////////////////////////////////////////
  // Scene
  ID rootEntity = loadScene("mesh/blender_chan/Sketchfab_2017_02_12_14_36_10.fbx", entityManager,
	  sceneObjects, renderables, lights, pool);
  SceneObject *rootSceneObj = sceneObjects.get(rootEntity);
  if (rootSceneObj) {
    rootSceneObj->localTransform.scaling = vec3{1.0f};
    rootSceneObj->calculateWorldTransform();
    rootSceneObj->calculateWorldBounds();
    camCtl.centerOnObject(rootSceneObj->getApproximateWorldBounds());
  }
  ID selectedItemID = 0;

  ////////////////////////////////////////////////////
  // Render
  w.onRender([&](ag::Window &win, double dt) {
    ////////////////////////////////////////////////////
    auto screenSizeI = win.getFramebufferSize();
    auto screenSize = vec2{(float)screenSizeI.x, (float)screenSizeI.y};
    camCtl.setAspectRatio(screenSize.x / screenSize.y);
    Camera cam = camCtl.getCamera();
    auto &fbo = gl::getDefaultFramebuffer();
    ////////////////////////////////////////////////////
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(0, 0, screenSizeI.x, screenSizeI.y);
    glClearColor(20.f / 255.f, 20.f / 255.f, 20.f / 255.f, 1.0f);
    glClearDepth(1.0);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    {
      AG_GPU_PROFILE_SCOPE("Draw grid")
      RenderUtils::drawGrid2D(fbo, screenSize / 2.0f, vec2{25.0f}, 10);
    }

	{
		// show scene editor
		AG_PROFILE_SCOPE("Scene editor")
		if (sceneEditor)
			sceneEditor->onSceneEditorGUI(scene, selectedItemID, cam, pool);
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
		  { "None", (int)DeferredSceneRenderer::DebugRenderMode::None },
		  { "Normals", (int)DeferredSceneRenderer::DebugRenderMode::Normals },
		  { "ObjectID", (int)DeferredSceneRenderer::DebugRenderMode::ObjectID },
		  { "Depth", (int)DeferredSceneRenderer::DebugRenderMode::Depth },
		  { "Positions", (int)DeferredSceneRenderer::DebugRenderMode::Positions },
		  { "Albedo", (int)DeferredSceneRenderer::DebugRenderMode::Albedo },
		  { "Velocity", (int)DeferredSceneRenderer::DebugRenderMode::Velocity },
	  };
	  static DeferredSceneRenderer::DebugRenderMode mode = DeferredSceneRenderer::DebugRenderMode::None;
	  gui::enumComboBoxT<DeferredSceneRenderer::DebugRenderMode>("debug deferred", &mode, names);
      deferredSceneRenderer.renderScene(fbo, sceneObjects, renderables, lights, cam, mode);
    }

    /*{
      AG_GPU_PROFILE_SCOPE("Rendering/Debug")
      for (auto &idSceneObj : scene.getObjects()) {
        auto &sceneObj = idSceneObj.second;
        if (sceneObj.mesh) {
                        RenderUtils::drawMesh(gl::getDefaultFramebuffer(), cam,
    *sceneObj.mesh, sceneObj.worldTransform);
        }
      }
    }*/
  });

  ////////////////////////////////////////////////////
  // window events
  w.onEvent([&](ag::Window &win, const ag::Event &ev) {
    if (ev.type == EventType::WindowResize) {
      deferredSceneRenderer.resize(ev.resize.width, ev.resize.height);
    }
  });

  w.show();
  return 0;
}
