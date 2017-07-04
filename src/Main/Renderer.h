#include <autograph/Core/Camera.h>
#include <autograph/Engine/FrameGraph.h>
#include <autograph/Engine/imgui.h>
#include <autograph/Engine/EntityManager.h>
#include <autograph/Engine/SceneObject.h>
#include <autograph/Engine/Renderable.h>
#include <autograph/Engine/Light.h>
#include <autograph/Gfx/All.h>
#include <glm/gtc/matrix_transform.hpp>

namespace ag {

/////////////////////////////////////
struct CameraMatrices {
  mat4 viewMatrix;
  mat4 projMatrix;
  mat4 viewProjMatrix;
  mat4 invProjMatrix;
  mat4 prevViewProjMatrixVelocity;
  mat4 viewProjMatrixVelocity;
};

struct CameraFrameData {
  CameraMatrices matrices;
  Buffer::Slice cameraUBO;
  vec2 TAAOffset;
};

struct GeometryBuffers {
  FrameGraph::Resource depth;   // D32_SFLOAT
  FrameGraph::Resource normals; // R16G16_SFLOAT
  FrameGraph::Resource diffuse; // R8G8B8A8
  FrameGraph::Resource objectIDs;
  FrameGraph::Resource velocity;
};

/////////////////////////////////////
// Build camera data
// TemporalAntiAliasingPass.cpp
CameraFrameData makeCameraFrameData(const Camera &camera, Camera &prevCamera,
	int width, int height,
	int TAACurrentSample);

/////////////////////////////////////
// Returns cleared G-buffers
// GeometryPass.cpp
GeometryBuffers initializeGeometryBuffers(FrameGraph &frameGraph, int width,
                                          int height);

/////////////////////////////////////
// GeometryPass.cpp
// Keeps state internally
struct GeometryRenderData {
  mat4 prevModelMat;
};
using GeometryRenderComponents = ComponentManager<GeometryRenderData>;

class GeometryPass {
public:
  GeometryBuffers addPass(FrameGraph &frameGraph, const GeometryBuffers &in,
                          SceneObjectComponents &sceneObjects,
                          RenderableComponents &renderables,
                          LightComponents &lights, const Camera &cam,
                          const CameraFrameData &camData);

private:
  GeometryRenderComponents renderComponents;
  Camera prevCamera;
};

/////////////////////////////////////
// DeferredEvalPass.cpp
FrameGraph::Resource addDeferredEvalPass(FrameGraph &frameGraph, int width,
	int height, const CameraFrameData &camData,
	const GeometryBuffers &gbuffers,
	int debugMode);

/////////////////////////////////////
// TemporalAntiAliasingPass.cpp
class TemporalAntiAliasingPass {
public:
  FrameGraph::Resource addPass(FrameGraph &frameGraph,
                               FrameGraph::Resource input,
                               FrameGraph::Resource velocity,
                               FrameGraph::Resource depth,
                               const CameraFrameData &camdata);

private:
  void resize(int width, int height);
  void runPass(Texture &in, Texture &velocity, Texture &depth, Texture &output,
               const CameraFrameData &camdata);

  Texture taa_history;
  Texture taa_tmp;
  Texture taa_debug;
  int taa_cur_sample = 0;
};

} // namespace ag
