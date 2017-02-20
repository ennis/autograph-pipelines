#pragma once
#include <autograph/Camera.h>
#include <autograph/engine/Scene.h>
#include <autograph/engine/RenderableScene.h>
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
  struct GBuffer {
  public:
    GBuffer() {}
    GBuffer(int width, int height);
    void release();
    gl::Texture &getDepthTarget() { return depthStencil; }
    gl::Texture &getNormalsTarget() { return normals; }
    gl::Texture &getDiffuseTarget() { return diffuse; }
    gl::Framebuffer &getFramebuffer() { return fbo; }

  private:
    gl::Texture depthStencil; // D32_SFLOAT
    gl::Texture normals;      // R10G10B10A2
    gl::Texture diffuse;      // R8G8B8A8
    gl::Framebuffer fbo;
  };

  DeferredSceneRenderer();
  ~DeferredSceneRenderer();

  void reloadShaders();
  void renderScene(GBuffer &targets, Scene &scene, RenderableScene& renderableScene, Camera &camera);

private:
  Shader deferredShader;
};

class WireframeOverlayRenderer {
public:
  WireframeOverlayRenderer();
  ~WireframeOverlayRenderer();

  void reloadShaders();
  void renderSceneObject(gl::Framebuffer &target, Scene &scene,
                         SceneObject &object, Camera &camera,
                         bool depthTest = true);

private:
  Shader wireframeShader;
  Shader wireframeNoDepthShader;
};
}
