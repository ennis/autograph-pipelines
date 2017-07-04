#include "Renderer.h"
#include <autograph/Engine/Shader.h>
#include <autograph/Engine/RenderUtils.h>

namespace ag
{
/////////////////////////////////////////////////////
// DEFERRED EVALUATION PASS
FrameGraph::Resource addDeferredEvalPass(FrameGraph &frameGraph, int width,
                                         int height, const CameraFrameData &camData,
                                         const GeometryBuffers &gbuffers,
                                         int debugMode)
{
  struct DeferredPassData
  {
    FrameGraph::Resource depth;
    FrameGraph::Resource normals;
    FrameGraph::Resource diffuse;
    FrameGraph::Resource objectIDs;
    FrameGraph::Resource velocity;
    FrameGraph::Resource output;
  };

  static GPUPipeline deferredPassShader = GPUPipeline{
      GPUPipelineType::Graphics, "shaders/default.lua$deferredPass",
      GPUPipeline::LoadMask::All, &Cache::getDefault()};

  auto &evalPass = frameGraph.addPass<DeferredPassData>(
      //////////////////////////////////////////////////////
      // SETUP
      [&](FrameGraph::PassBuilder &builder, DeferredPassData &data) {
        data.depth = builder.read(gbuffers.depth);
        data.normals = builder.read(gbuffers.normals);
        data.diffuse = builder.read(gbuffers.diffuse);
        data.objectIDs = builder.read(gbuffers.objectIDs);
        data.velocity = builder.read(gbuffers.velocity);
        data.output = builder.createTexture2D(ImageFormat::R16G16B16A16_SFLOAT,
                                              width, height, "output");
        builder.setName("DeferredEvalPass");
      },
      //////////////////////////////////////////////////////
      // EXECUTE
      [=](DeferredPassData &data, FrameGraph::PassResources &pass) {
        auto &sm_nearest = Sampler::nearestRepeat();
        // make FBO
        Framebuffer fbo;
        fbo.setAttachement(gl::COLOR_ATTACHMENT0 + 0,
                           pass.getTexture(data.output));
        drawQuad(
            fbo, deferredPassShader,
            bind::texture(0, pass.getTexture(data.diffuse), sm_nearest),
            bind::texture(1, pass.getTexture(data.normals), sm_nearest),
            bind::texture(2, pass.getTexture(data.objectIDs), sm_nearest),
            bind::texture(3, pass.getTexture(data.velocity), sm_nearest),
            bind::texture(4, pass.getTexture(data.depth), sm_nearest),
            bind::uniformBuffer(0, camData.cameraUBO),
            bind::uniform_int("uDebugMode", 2));
      });
  return evalPass.output;
}
}