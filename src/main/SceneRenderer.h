#pragma once
#include <autograph/Camera.h>
#include <autograph/engine/RenderableScene.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/Shader.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/UploadBuffer.h>

namespace ag {

// scene renderer
// Renders G-buffers
class DeferredSceneRenderer {
public:
  DeferredSceneRenderer(int width, int height);
  ~DeferredSceneRenderer();

  enum class DebugRenderMode {
	  None = 0,
	  Normals = 1,
	  ObjectID = 2,
	  Depth = 3,
	  Positions = 4,
	  Albedo = 5,
	  Velocity = 6,
  };

  void resize(int width, int height);
  gl::Texture &getDepthTarget() { return depth; }
  gl::Texture &getNormalsTarget() { return normals; }
  gl::Texture &getDiffuseTarget() { return diffuse; }
  gl::Framebuffer &getFramebuffer() { return fbo; }

  void reloadShaders();
  void renderScene(gl::Framebuffer& target, Scene &scene, RenderableScene &renderableScene,
	  const Camera &camera, DebugRenderMode debugRenderMode = DebugRenderMode::None);

private:
  gl::Texture depth; // D32_SFLOAT
  gl::Texture normals;      // R16G16_SFLOAT
  gl::Texture diffuse;      // R8G8B8A8
  gl::Texture objectIDs;
  gl::Texture velocity;
  gl::Texture deferredTarget;	// R16G16B16A16_SFLOAT
  gl::Texture TAATarget0;
  gl::Texture TAATarget1;

  Camera prevCamera;
  int TAACurrentSample = 0;
  gl::Framebuffer fbo;
  gl::Framebuffer deferredTargetFbo;
  Shader geometryPassShader;
  Shader deferredPassShader;
  Shader TAAAverageShader;

  struct ObjectRenderData {
	  mat4 prevModelMat;
  };

  ComponentManager<ObjectRenderData> objectsRenderData;
};

class WireframeOverlayRenderer {
public:
  WireframeOverlayRenderer();
  ~WireframeOverlayRenderer();

  void reloadShaders();
  void renderSceneObject(gl::Framebuffer &target, Scene &scene,
                         SceneObject &object, const Camera &camera,
                         bool depthTest = true);

private:
  Shader wireframeShader;
  Shader wireframeNoDepthShader;
};
}
