#include <autograph/Camera.h>
#include <autograph/engine/Light.h>
#include <autograph/engine/Profiler.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/Renderable.h>
#include <autograph/engine/SceneObject.h>
#include <autograph/engine/Shader.h>
#include <autograph/engine/ShaderUtils.h>
#include <autograph/gl/Compute.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <glm/gtc/matrix_transform.hpp>

extern "C" __declspec(dllexport) void moduleInit()
{
	AG_DEBUG("Module init!");
	// TODO: register stuff
}

extern "C" __declspec(dllexport) void moduleExit()
{
	AG_DEBUG("Module exit!");
}

namespace ag {

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
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
  void renderScene(gl::Framebuffer &target, SceneObjectComponents &sceneObjects,
                   RenderableComponents &renderables, LightComponents &lights,
                   const Camera &camera,
                   DebugRenderMode debugRenderMode = DebugRenderMode::None);

  auto &getRenderData() { return objectsRenderData; }

private:
  gl::Texture depth;   // D32_SFLOAT
  gl::Texture normals; // R16G16_SFLOAT
  gl::Texture diffuse; // R8G8B8A8
  gl::Texture objectIDs;
  gl::Texture velocity;
  gl::Texture deferredTarget; // R16G16B16A16_SFLOAT
  gl::Texture TAATarget0;
  gl::Texture TAATarget1;
  gl::Texture TAADebug;

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



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
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

void DeferredSceneRenderer::renderScene(gl::Framebuffer &target,
                                        SceneObjectComponents &sceneObjects,
                                        RenderableComponents &renderables,
                                        LightComponents &lights,
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
  vec2 TAAOffset =
      TAASampleScale * TAASampleOffsets[TAACurrentSample % TAASampleCount];
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
  cam.projMatrix =
      glm::translate(mat4{1.0f}, vec3{2.0f * TAAOffset.x / fWidth,
                                      2.0f * TAAOffset.y / fHeight, 0.0f}) *
      camera.projMat;
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

  auto &objects = sceneObjects.getObjects();

  int objectID = 0;

  {
    AG_GPU_PROFILE_SCOPE("Geometry pass")
    for (auto &&kv : objects) {

      auto id = kv.first;
      auto &sceneObj = kv.second;
      auto material = renderables.get(id);
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
        texture(2, velocity, nearestSampler), texture(3, depth, linearSampler),
        image(0, TAATarget1), image(1, TAADebug), uniformBuffer(0, uCamera),
        uniform_float("uWeight", TAAWeight),
        uniform_vec2("uSampleOffset", TAAOffset));
    ++TAACurrentSample;
  }

  {
    AG_GPU_PROFILE_SCOPE("Copy to target")
    RenderUtils::drawTexturedQuad(target, TAATarget1);
  }
  std::swap(TAATarget0, TAATarget1);
}
}
