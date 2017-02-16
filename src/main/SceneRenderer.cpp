#include "SceneRenderer.h"
#include <autograph/engine/Shader.h>
#include <autograph/engine/ShaderUtils.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>

namespace ag {
DeferredSceneRenderer::GBuffer::GBuffer(int width, int height) {
  depthStencil = gl::Texture::create2D(ImageFormat::D32_SFLOAT, width, height);
  diffuse = gl::Texture::create2D(ImageFormat::R8G8B8A8_UNORM, width, height);
  normals = gl::Texture::create2D(ImageFormat::A2R10G10B10_UNORM_PACK32, width,
                                  height); // SNORM not supported in OpenGL
  fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 0, diffuse);
  fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 1, normals);
  fbo.setAttachement(GL_DEPTH_ATTACHMENT, depthStencil);
}

void DeferredSceneRenderer::GBuffer::release() {
  depthStencil = {};
  diffuse = {};
  normals = {};
  fbo = {};
}

DeferredSceneRenderer::DeferredSceneRenderer() { reloadShaders(); }

DeferredSceneRenderer::~DeferredSceneRenderer() {}

void DeferredSceneRenderer::reloadShaders() {
  deferredShader = Shader{"shaders/default:deferred"};
}

void DeferredSceneRenderer::renderScene(GBuffer &targets, EntityList &scene,
                                        Camera &camera) {
  // Per-object uniforms
  CameraPerObjectUniforms camUniforms{camera};
  gl::clearTexture(targets.getDiffuseTarget(), vec4{0.0f, 0.0f, 0.0f, 1.0f});
  gl::clearDepthTexture(targets.getDepthTarget(), 1.0f);
  camUniforms.viewMatrix = camera.viewMat;
  camUniforms.projMatrix = camera.projMat;
  camUniforms.viewProjMatrix = camera.projMat * camera.viewMat;

  auto &objects = scene.getObjects();
  for (auto &&kv : objects) {
    auto id = kv.first;
    auto scene_obj = kv.second.getComponent<SceneObject>();
    auto scene_renderable = kv.second.getComponent<Renderable>();
    if (!scene_obj)
      return;
    auto pMesh = scene_obj->mesh;
    if (!pMesh)
      continue;
    using namespace gl;
    using namespace gl::bind;
    camUniforms.modelMatrix = scene_obj->worldTransform;
    draw(targets.getFramebuffer(), *pMesh, deferredShader,
         uniformFrameData(0, &camUniforms));

    // AG_DEBUG("renderScene, object ID {} mesh {}", obj->id, (void*)obj->mesh);
  }
}

WireframeOverlayRenderer::WireframeOverlayRenderer() { reloadShaders(); }

WireframeOverlayRenderer::~WireframeOverlayRenderer() {}

void WireframeOverlayRenderer::reloadShaders() {
  wireframeShader = Shader{"shaders/default:drawWireMesh"};
  wireframeNoDepthShader = Shader{"shaders/default:drawWireMeshNoDepth"};
}

// render one scene object and its children
void WireframeOverlayRenderer::renderSceneObject(gl::Framebuffer &target,
                                                 EntityList &scene,
                                                 SceneObject &object,
                                                 Camera &camera,
                                                 bool depthTest) {
  auto pMesh = object.mesh;

  if (pMesh) {
    // Per-object uniforms
    struct ObjectUniforms {
      mat4 modelMatrix;
      mat4 viewMatrix;
      mat4 projMatrix;
      mat4 viewProjMatrix;
    } objectUniforms;

    objectUniforms.modelMatrix = object.worldTransform;
    objectUniforms.viewMatrix = camera.viewMat;
    objectUniforms.projMatrix = camera.projMat;
    objectUniforms.viewProjMatrix = camera.projMat * camera.viewMat;

    using namespace gl;
    using namespace gl::bind;
    auto pShader = depthTest ? &wireframeShader : &wireframeNoDepthShader;

    draw(target, *pMesh, *pShader,
         uniformBuffer(
             0, uploadFrameData(&objectUniforms, sizeof(objectUniforms))));
  }
  for (auto c : object.children) {
    renderSceneObject(target, scene, *c, camera, depthTest);
  }
}
}
