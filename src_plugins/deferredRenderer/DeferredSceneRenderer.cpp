#include "DeferredSceneRenderer.Config.h"
#include <autograph/Camera.h>
#include <autograph/editor/SceneRenderer.h>
#include <autograph/engine/All.h>
#include <autograph/engine/imgui.h>
#include <autograph/gl/All.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <glm/gtc/matrix_transform.hpp>

namespace ag {

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

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class DeferredSceneRenderer_API DeferredSceneRenderer : public SceneRenderer {
public:
  DeferredSceneRenderer(int width = 640, int height = 480) {
    resize(width, height);
    reloadShaders();
  }

  ~DeferredSceneRenderer() {}

  virtual void onGUI() override {
    ImGui::Begin("Deferred scene renderer");
    ImGui::Text("Deferred scene renderer! Live update!");
    ImGui::Button("Another widget");
    ImGui::End();
  }

  void resize(int width, int height) {
    fbo = {};
    depth = {};
    diffuse = {};
    normals = {};
    objectIDs = {};
    velocity = {};
    depth = Texture::create2D(ImageFormat::D32_SFLOAT, width, height);
    diffuse = Texture::create2D(ImageFormat::R8G8B8A8_UNORM, width, height);
    normals = Texture::create2D(ImageFormat::R16G16_SFLOAT, width, height);
    objectIDs = Texture::create2D(ImageFormat::R16G16_SINT, width, height);
    velocity = Texture::create2D(ImageFormat::R32G32_SFLOAT, width, height);
    fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 0, diffuse);
    fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 1, normals);
    fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 2, objectIDs);
    fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 3, velocity);
    fbo.setAttachement(gl::DEPTH_ATTACHMENT, depth);

    deferredTargetFbo = {};
    deferredTarget = {};
    deferredTarget =
        Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
    deferredTargetFbo.setAttachement(gl::COLOR_ATTACHMENT0 + 0, deferredTarget);

    TAATarget0 = {};
    TAATarget1 = {};
    TAATarget0 =
        Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
    TAATarget1 =
        Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
    TAADebug =
        Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);

    TAACurrentSample = 0;
  }

  Texture &getDepthTarget() { return depth; }
  Texture &getNormalsTarget() { return normals; }
  Texture &getDiffuseTarget() { return diffuse; }
  Framebuffer &getFramebuffer() { return fbo; }

  void reloadShaders() {
    geometryPassShader = Shader{"shaders/default:geometryPass"};
    deferredPassShader = Shader{"shaders/default:deferredPass"};
    TAAAverageShader = Shader{"shaders/default:TAAAverage"};
  }

  void renderScene(Framebuffer &target, SceneObjectComponents &sceneObjects,
                   RenderableComponents &renderables, LightComponents &lights,
                   const Camera &camera,
                   SceneRenderer::DebugRenderMode debugRenderMode =
                       DebugRenderMode::None) override {

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
    auto uCamera = uploadFrameData(&cam, sizeof(cam));
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

        Texture *albedoTex = material ? material->albedo : nullptr;
        draw(getFramebuffer(), *pMesh, geometryPassShader,
             bind::uniformBuffer(0, uCamera),
             bind::texture(0, albedoTex ? albedoTex->object() : 0,
                           linearSampler.object()),
             bind::uniform_mat4("uPrevModelMatrix", renderData->prevModelMat),
             bind::uniform_mat4("uModelMatrix", sceneObj.worldTransform),
             bind::uniform_int("uObjectID", objectID));
        renderData->prevModelMat = sceneObj.worldTransform;
        ++objectID;
        // AG_DEBUG("renderScene, object ID {} mesh {}", obj->id,
        // (void*)obj->mesh);
      }
    }

    {
      AG_GPU_PROFILE_SCOPE("Deferred evaluation pass")
      drawQuad(deferredTargetFbo, deferredPassShader,
               bind::texture(0, diffuse, nearestSampler),
               bind::texture(1, normals, nearestSampler),
               bind::texture(2, objectIDs, nearestSampler),
               bind::texture(3, velocity, nearestSampler),
               bind::texture(4, depth, nearestSampler),
               bind::uniformBuffer(0, uCamera),
               bind::uniform_int("uDebugMode", (int)debugRenderMode));
    }

    {
      AG_GPU_PROFILE_SCOPE("Temporal AA pass")
      // Apply TAA
      float TAAWeight =
          (TAACurrentSample == 0) ? 1.0f : (1.0f / TAASampleCount);
      dispatchComputeOnImage2D(
          TAATarget0.width(), TAATarget0.height(), 8, 8, TAAAverageShader,
          bind::texture(0, TAATarget0, linearSampler),
          bind::texture(1, deferredTarget, nearestSampler),
          bind::texture(2, velocity, nearestSampler),
          bind::texture(3, depth, linearSampler), bind::image(0, TAATarget1),
          bind::image(1, TAADebug), bind::uniformBuffer(0, uCamera),
          bind::uniform_float("uWeight", TAAWeight),
          bind::uniform_vec2("uSampleOffset", TAAOffset));
      ++TAACurrentSample;
    }

    {
      AG_GPU_PROFILE_SCOPE("Copy to target")
      RenderUtils::drawTexturedQuad(target, TAATarget1);
    }
    std::swap(TAATarget0, TAATarget1);
  }

  auto &getRenderData() { return objectsRenderData; }

private:
  Texture depth;   // D32_SFLOAT
  Texture normals; // R16G16_SFLOAT
  Texture diffuse; // R8G8B8A8
  Texture objectIDs;
  Texture velocity;
  Texture deferredTarget; // R16G16B16A16_SFLOAT
  Texture TAATarget0;
  Texture TAATarget1;
  Texture TAADebug;

  Camera prevCamera;
  int TAACurrentSample = 0;
  Framebuffer fbo;
  Framebuffer deferredTargetFbo;
  Shader geometryPassShader;
  Shader deferredPassShader;
  Shader TAAAverageShader;

  struct ObjectRenderData {
    mat4 prevModelMat;
  };

  ComponentManager<ObjectRenderData> objectsRenderData;
};
} // namespace ag

PLUGIN_ENTRY {
  ag::registerClass<ag::DeferredSceneRenderer, ag::SceneRenderer>(
      "DeferredSceneRenderer");
}
