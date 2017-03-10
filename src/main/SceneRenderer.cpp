#include "SceneRenderer.h"
#include <autograph/engine/Profiler.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/Shader.h>
#include <autograph/engine/ShaderUtils.h>
#include <autograph/gl/Compute.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>

#include <glm/gtc/matrix_transform.hpp>

namespace ag {

// AA sample mask
static constexpr float TAASampleScale = 1.0f / 16.0f;
static constexpr int TAASampleCount = 32;
static vec2 TAASampleOffsets[TAASampleCount] = {
    {-4.0f, -7.0f}, {-7.0f, -5.0f}, {-3.0f, -5.0f}, {-5.0f, -4.0f},
    {-1.0f, -4.0f}, {-2.0f, -2.0f}, {-6.0f, -1.0f}, {-4.0f, 0.0f},
    {-7.0f, 1.0f},  {-1.0f, 2.0f},  {-6.0f, 3.0f},  {-3.0f, 3.0f},
    {-7.0f, 6.0f},  {-3.0f, 6.0f},  {-5.0f, 7.0f},  {-1.0f, 7.0f},
    {5.0f, -7.0f},  {1.0f, -6.0f},  {6.0f, -5.0f},  {4.0f, -4.0f},
    {2.0f, -3.0f},  {7.0f, -2.0f},  {1.0f, -1.0f},  {4.0f, -1.0f},
    {2.0f, 1.0f},   {6.0f, 2.0f},   {0.0f, 4.0f},   {4.0f, 4.0f},
    {2.0f, 5.0f},   {7.0f, 5.0f},   {5.0f, 6.0f},   {3.0f, 7.0f}};

DeferredSceneRenderer::DeferredSceneRenderer(int width, int height) {
  resize(width, height);
  reloadShaders();
}

DeferredSceneRenderer::~DeferredSceneRenderer() {}

void DeferredSceneRenderer::reloadShaders() {
  geometryPassShader = Shader{"shaders/default:geometryPass"};
  deferredPassShader = Shader{"shaders/default:deferredPass"};
  TAAAverageShader = Shader{"shaders/default:TAAAverage"};
}

void DeferredSceneRenderer::resize(int width, int height) {
  fbo = {};
  depth = {};
  diffuse = {};
  normals = {};
  objectIDs = {};
  velocity = {};
  depth = gl::Texture::create2D(ImageFormat::D32_SFLOAT, width, height);
  diffuse = gl::Texture::create2D(ImageFormat::R8G8B8A8_UNORM, width, height);
  normals = gl::Texture::create2D(ImageFormat::R16G16_SFLOAT, width, height);
  objectIDs = gl::Texture::create2D(ImageFormat::R16G16_SINT, width, height);
  velocity = gl::Texture::create2D(ImageFormat::R32G32_SFLOAT, width, height);
  fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 0, diffuse);
  fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 1, normals);
  fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 2, objectIDs);
  fbo.setAttachement(GL_COLOR_ATTACHMENT0 + 3, velocity);
  fbo.setAttachement(GL_DEPTH_ATTACHMENT, depth);

  deferredTargetFbo = {};
  deferredTarget = {};
  deferredTarget =
      gl::Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
  deferredTargetFbo.setAttachement(GL_COLOR_ATTACHMENT0 + 0, deferredTarget);

  TAATarget0 = {};
  TAATarget1 = {};
  TAATarget0 =
      gl::Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
  TAATarget1 =
      gl::Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
  TAADebug =
	  gl::Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);

  TAACurrentSample = 0;
}

/*static mat4 getJitteredProjectionMatrix(const Camera& cam)
{
	float vertical = Mathf.Tan(0.5f * Mathf.Deg2Rad * cam.fieldOfView);
	float horizontal = vertical * camera.aspect;

	offset.x *= horizontal / (0.5f * camera.pixelWidth);
	offset.y *= vertical / (0.5f * camera.pixelHeight);

	float left = (offset.x - horizontal) * camera.nearClipPlane;
	float right = (offset.x + horizontal) * camera.nearClipPlane;
	float top = (offset.y + vertical) * camera.nearClipPlane;
	float bottom = (offset.y - vertical) * camera.nearClipPlane;

	mat4 matrix;

	matrix[0, 0] = (2.0f * camera.nearClipPlane) / (right - left);
	matrix[0, 1] = 0.0f;
	matrix[0, 2] = (right + left) / (right - left);
	matrix[0, 3] = 0.0f;

	matrix[1, 0] = 0.0f;
	matrix[1, 1] = (2.0f * camera.nearClipPlane) / (top - bottom);
	matrix[1, 2] = (top + bottom) / (top - bottom);
	matrix[1, 3] = 0.0f;

	matrix[2, 0] = 0.0f;
	matrix[2, 1] = 0.0f;
	matrix[2, 2] = -(camera.farClipPlane + camera.nearClipPlane) / (camera.farClipPlane - camera.nearClipPlane);
	matrix[2, 3] = -(2.0f * camera.farClipPlane * camera.nearClipPlane) / (camera.farClipPlane - camera.nearClipPlane);

	matrix[3, 0] = 0.0f;
	matrix[3, 1] = 0.0f;
	matrix[3, 2] = -1.0f;
	matrix[3, 3] = 0.0f;

	return matrix;
}*/

void DeferredSceneRenderer::renderScene(gl::Framebuffer &target, Scene &scene,
                                        RenderableScene &renderableScene,
                                        const Camera &camera,
                                        DebugRenderMode debugRenderMode) {
  using namespace gl;
  using namespace gl::bind;
  auto &linearSampler = RenderUtils::getLinearSampler();
  auto &nearestSampler = RenderUtils::getNearestSampler();

  // Camera and temporal AA setup
  if (TAACurrentSample == 0) {
    prevCamera = camera;
  }
  float fWidth = (float)diffuse.width();
  float fHeight = (float)diffuse.height();
  vec2 TAAOffset = TAASampleScale *
                   TAASampleOffsets[TAACurrentSample % TAASampleCount];
 // vec2 TAAOffset{ 0.0f };
  struct CameraMatrices {
    mat4 viewMatrix;
    mat4 projMatrix;
    mat4 viewProjMatrix;
    mat4 invProjMatrix;
    mat4 prevViewProjMatrixVelocity;
    mat4 viewProjMatrixVelocity;
  };
  CameraMatrices cam;
  cam.projMatrix = glm::translate(mat4{ 1.0f }, vec3{ 2.0f * TAAOffset.x / fWidth,
                                                   2.0f * TAAOffset.y / fHeight, 0.0f}) * camera.projMat;
  cam.viewMatrix = camera.viewMat;
  cam.viewProjMatrix = cam.projMatrix * cam.viewMatrix;
  cam.invProjMatrix = glm::inverse(cam.projMatrix);
  // Projection matrices for velocity calculation
  cam.prevViewProjMatrixVelocity = prevCamera.projMat * prevCamera.viewMat;
  cam.viewProjMatrixVelocity = camera.projMat * camera.viewMat;
  auto uCamera = gl::uploadFrameData(&cam, sizeof(cam));
  prevCamera = camera;

  // Clear targets
  clearTexture(diffuse, vec4{0.0f, 0.0f, 0.0f, 1.0f});
  clearTexture(normals, vec4{0.5f, 0.5f, 0.0f, 1.0f});
  clearTexture(objectIDs, ivec4{0, 0, 0, 0});
  clearTexture(deferredTarget, vec4{0.0f, 0.0f, 0.0f, 1.0f});
  clearTexture(velocity, vec4{0.0f, 0.0f, 0.0f, 1.0f});
  clearDepthTexture(depth, 1.0f);

  auto &objects = scene.getObjects();

  int objectID = 0;

  {
    AG_GPU_PROFILE_SCOPE("Geometry pass")
    for (auto &&kv : objects) {

      auto id = kv.first;
      auto &sceneObj = kv.second;
      auto material = renderableScene.get(id);
      auto pMesh = sceneObj.mesh;
      if (!pMesh)
        continue;
      // get or create per-object render data
      auto renderData = objectsRenderData.get(id);
      if (!renderData) {
        renderData = objectsRenderData.add(id);
        // init prev transform
        renderData->prevModelMat = sceneObj.worldTransform;
      }

      gl::Texture *albedoTex = material ? material->albedo : nullptr;
      draw(getFramebuffer(), *pMesh, geometryPassShader,
           uniformBuffer(0, uCamera),
           texture(0, albedoTex ? albedoTex->object() : 0,
                   linearSampler.object()),
           uniform_mat4("uPrevModelMatrix", renderData->prevModelMat),
           uniform_mat4("uModelMatrix", sceneObj.worldTransform),
           uniform_int("uObjectID", objectID));
      renderData->prevModelMat = sceneObj.worldTransform;
      ++objectID;
      // AG_DEBUG("renderScene, object ID {} mesh {}", obj->id,
      // (void*)obj->mesh);
    }
  }

  {
    AG_GPU_PROFILE_SCOPE("Deferred evaluation pass")
    gl::drawQuad(deferredTargetFbo, deferredPassShader,
                 texture(0, diffuse, nearestSampler),
                 texture(1, normals, nearestSampler),
                 texture(2, objectIDs, nearestSampler),
                 texture(3, velocity, nearestSampler),
                 texture(4, depth, nearestSampler), uniformBuffer(0, uCamera),
                 uniform_int("uDebugMode", (int)debugRenderMode));
  }

  {
    AG_GPU_PROFILE_SCOPE("Temporal AA pass")
    // Apply TAA
    float TAAWeight = (TAACurrentSample == 0) ? 1.0f : (1.0f / TAASampleCount);
    gl::dispatchComputeOnImage2D(
        TAATarget0.width(), TAATarget0.height(), 8, 8, TAAAverageShader,
        texture(0, TAATarget0, linearSampler),
        texture(1, deferredTarget, nearestSampler),
		texture(2, velocity, nearestSampler),
		texture(3, depth, linearSampler),
		image(0, TAATarget1),
		image(1, TAADebug),
        uniformBuffer(0, uCamera), uniform_float("uWeight", TAAWeight),
        uniform_vec2("uSampleOffset", TAAOffset));
    ++TAACurrentSample;
  }

  {
    AG_GPU_PROFILE_SCOPE("Copy to target")
    RenderUtils::drawTexturedQuad(target, TAATarget1);
  }
  std::swap(TAATarget0, TAATarget1);
}

WireframeOverlayRenderer::WireframeOverlayRenderer() { reloadShaders(); }

WireframeOverlayRenderer::~WireframeOverlayRenderer() {}

void WireframeOverlayRenderer::reloadShaders() {
  wireframeShader = Shader{"shaders/default:drawWireMesh"};
  wireframeNoDepthShader = Shader{"shaders/default:drawWireMeshNoDepth"};
}

// render one scene object and its children
void WireframeOverlayRenderer::renderSceneObject(gl::Framebuffer &target,
                                                 Scene &scene,
                                                 SceneObject &object,
                                                 const Camera &camera,
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
