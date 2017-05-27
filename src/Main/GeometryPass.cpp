#include "Renderer.h"

namespace ag {

GeometryBuffers initializeGeometryBuffers(FrameGraph &frameGraph, int width,
                                          int height) {

  return frameGraph.addPass<GeometryBuffers>(
      //////////////////////////////////////////////////////
      // SETUP
      [&](FrameGraph::PassBuilder &builder, GeometryBuffers &data) {
        data.depth = builder.createTexture2D(ImageFormat::D32_SFLOAT, width,
                                             height, "depth");
        data.diffuse = builder.createTexture2D(ImageFormat::R8G8B8A8_UNORM,
                                               width, height, "diffuse");
        data.normals = builder.createTexture2D(ImageFormat::R16G16_SFLOAT,
                                               width, height, "normals");
        data.objectIDs = builder.createTexture2D(ImageFormat::R16G16_SINT,
                                                 width, height, "objectIDs");
        data.velocity = builder.createTexture2D(ImageFormat::R32G32_SFLOAT,
                                                width, height, "velocity");
        builder.setName("InitGeometryBuffers");
      },
      //////////////////////////////////////////////////////
      // EXECUTE
      [](GeometryBuffers &data, FrameGraph::PassResources &pass) {
        // Clear targets
        clearTexture(pass.getTexture(data.diffuse),
                     vec4{0.0f, 0.0f, 0.0f, 1.0f});
        clearTexture(pass.getTexture(data.normals),
                     vec4{0.5f, 0.5f, 0.0f, 1.0f});
        clearTexture(pass.getTexture(data.objectIDs), ivec4{0, 0, 0, 0});
        clearTexture(pass.getTexture(data.depth), vec4{0.0f, 0.0f, 0.0f, 1.0f});
        clearTexture(pass.getTexture(data.velocity),
                     vec4{0.0f, 0.0f, 0.0f, 1.0f});
      });
}

//
// This is the function that is in charge of rendering the (opaque) geometry of
// a scene in G-buffers

static void
runRenderScenePass(Texture &diffuse, Texture &normals, Texture &objectIDs,
                   Texture &velocity, Texture &depth,
                   SceneObjectComponents &sceneObjects,
                   RenderableComponents &renderables, LightComponents &lights,
                   GeometryRenderComponents &renderComponents,
                   const Camera &cam, const CameraFrameData &camData) {
  auto &sm_nearest = RenderUtils::getNearestSampler();
  auto &sm_linear = RenderUtils::getLinearSampler();
  // Find shader in cache
  GPUPipeline geometryPassShader =
      GPUPipeline{GPUPipelineType::Graphics, "shaders/default.lua$geometryPass",
                  GPUPipeline::LoadMask::All, &Cache::getDefault()};
  // Create the framebuffer
  Framebuffer fbo;
  fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 0, diffuse);
  fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 1, normals);
  fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 2, objectIDs);
  fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 3, velocity);
  fbo.setAttachement(gl::DEPTH_ATTACHMENT, depth);
  // Render all objects
  auto &objects = sceneObjects.getObjects();
  int objectID = 0;

  for (auto &&kv : objects) {
    auto id = kv.first;
    auto &sceneObj = kv.second;
    // Nothing to render if this scene object has no associated
    // mesh
    if (!sceneObj.mesh)
      continue;
    // Get material from renderables scene
    auto material = renderables.get(id);
    // get or create per-object render data
    auto renderData = renderComponents.get(id);
    if (!renderData) {
      renderData = renderComponents.add(id);
      // init prev transform
      renderData->prevModelMat = sceneObj.worldTransform;
    }
    // Get textures from the material associated to the object
    Texture *albedoTex = material ? material->albedo : nullptr;
    // Issue the draw call with the camera data, textures,
    // matrices, etc...
    draw(fbo, *sceneObj.mesh, geometryPassShader,
         bind::uniformBuffer(0, camData.cameraUBO),
         bind::texture(0, albedoTex ? albedoTex->object() : 0,
                       sm_linear.object()),
         bind::uniform_mat4("uPrevModelMatrix", renderData->prevModelMat),
         bind::uniform_mat4("uModelMatrix", sceneObj.worldTransform),
         bind::uniform_int("uObjectID", objectID));
    // Update the previous model matrix (for velocity
    // calculations)
    renderData->prevModelMat = sceneObj.worldTransform;
    // Next object
    ++objectID;
  }
}

GeometryBuffers GeometryPass::addPass(FrameGraph &frameGraph,
                                      const GeometryBuffers &in,
                                      SceneObjectComponents &sceneObjects,
                                      RenderableComponents &renderables,
                                      LightComponents &lights,
                                      const Camera &cam,
                                      const CameraFrameData &camData) {
  return frameGraph.addPass<GeometryBuffers>(
      //////////////////////////////////////////////////////
      // SETUP
      [&](FrameGraph::PassBuilder &builder, GeometryBuffers &data) {
        data.depth = builder.write(in.depth);
        data.diffuse = builder.write(in.diffuse);
        data.normals = builder.write(in.normals);
        data.objectIDs = builder.write(in.objectIDs);
        data.velocity = builder.write(in.velocity);
        builder.setName("GeometryPass");
      },
      //////////////////////////////////////////////////////
      // EXECUTE
      [this, &sceneObjects, &renderables, &lights, cam,
       camData](GeometryBuffers &data, FrameGraph::PassResources &pass) {
        runRenderScenePass(
            pass.getTexture(data.diffuse), pass.getTexture(data.normals),
            pass.getTexture(data.objectIDs), pass.getTexture(data.velocity),
            pass.getTexture(data.depth), sceneObjects, renderables, lights,
            this->renderComponents, cam, camData);
      });
}
} // namespace ag