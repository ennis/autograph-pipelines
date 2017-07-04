#include "Renderer.h"
#include <autograph/Engine/Shader.h>
#include <autograph/Engine/RenderUtils.h>

namespace ag
{

static constexpr float TAASampleScale = 1.0f / 16.0f;
static constexpr int TAASampleCount = 32;

static vec2 TAASampleOffsets[TAASampleCount] = {
    {-4.0f, -7.0f}, {-7.0f, -5.0f}, {-3.0f, -5.0f}, {-5.0f, -4.0f}, {-1.0f, -4.0f}, {-2.0f, -2.0f}, {-6.0f, -1.0f}, {-4.0f, 0.0f}, {-7.0f, 1.0f}, {-1.0f, 2.0f}, {-6.0f, 3.0f}, {-3.0f, 3.0f}, {-7.0f, 6.0f}, {-3.0f, 6.0f}, {-5.0f, 7.0f}, {-1.0f, 7.0f}, {5.0f, -7.0f}, {1.0f, -6.0f}, {6.0f, -5.0f}, {4.0f, -4.0f}, {2.0f, -3.0f}, {7.0f, -2.0f}, {1.0f, -1.0f}, {4.0f, -1.0f}, {2.0f, 1.0f}, {6.0f, 2.0f}, {0.0f, 4.0f}, {4.0f, 4.0f}, {2.0f, 5.0f}, {7.0f, 5.0f}, {5.0f, 6.0f}, {3.0f, 7.0f}};

// Setup a camera for rendering with TAA
CameraFrameData makeCameraFrameData(const Camera &camera, Camera &prevCamera,
                                    int width, int height,
                                    int TAACurrentSample)
{
    CameraFrameData cfd;
    //////////////////////////////////////////////////////
    // CAMERA VARS / TAA SETUP
    /*if (TAACurrentSample == 0) {
  prevCamera = camera;
  }*/
    float fWidth = (float)width;
    float fHeight = (float)height;
    cfd.TAAOffset =
        TAASampleScale * TAASampleOffsets[TAACurrentSample % TAASampleCount];
    // vec2 TAAOffset{ 0.0f };
    CameraMatrices mtx;
    mtx.projMatrix =
        glm::translate(mat4{1.0f}, vec3{2.0f * cfd.TAAOffset.x / fWidth,
                                        2.0f * cfd.TAAOffset.y / fHeight, 0.0f}) *
        camera.projMat;
    mtx.viewMatrix = camera.viewMat;
    mtx.viewProjMatrix = mtx.projMatrix * mtx.viewMatrix;
    mtx.invProjMatrix = glm::inverse(mtx.projMatrix);
    // Projection matrices for velocity calculation
    mtx.prevViewProjMatrixVelocity = prevCamera.projMat * prevCamera.viewMat;
    mtx.viewProjMatrixVelocity = camera.projMat * camera.viewMat;
    cfd.cameraUBO = uploadFrameData(&mtx, sizeof(mtx));
    prevCamera = camera;
    cfd.matrices = mtx;
    return cfd;
}

void TemporalAntiAliasingPass::resize(int width, int height)
{
    taa_history = {};
    taa_tmp = {};
    taa_history =
        Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
    taa_tmp = Texture::create2D(ImageFormat::R16G16B16A16_SFLOAT, width, height);
    taa_cur_sample = 0;
}

void TemporalAntiAliasingPass::runPass(Texture &in, Texture &velocity,
                                       Texture &depth, Texture &output,
                                       const CameraFrameData &camdata)
{
    int width = in.width();
    int height = in.height();
    auto &sm_linear = Sampler::linearRepeat();
    auto &sm_nearest = Sampler::nearestRepeat();
    static GPUPipeline TAAAverageShader =
        GPUPipeline{GPUPipelineType::Graphics, "shaders/default.lua$TAA_Average",
                    GPUPipeline::LoadMask::All, &Cache::getDefault()};
    // Apply TAA
    float taa_weight = (taa_cur_sample == 0) ? 1.0f : (1.0f / TAASampleCount);
    // Dispatch compute shader: read from taa_history, write to
    // taa_tmp
    dispatchComputeOnImage2D(
        width, height, 8, 8, TAAAverageShader,
        bind::texture(0, taa_history, sm_linear),
        bind::texture(1, in, sm_nearest), bind::texture(2, velocity, sm_nearest),
        bind::texture(3, depth, sm_linear), bind::image(0, taa_tmp),
        bind::image(1, taa_debug), bind::uniformBuffer(0, camdata.cameraUBO),
        bind::uniform_float("uWeight", taa_weight),
        bind::uniform_vec2("uSampleOffset", camdata.TAAOffset));
    ++taa_cur_sample;
    // Copy result to output
    Framebuffer fbo;
    fbo.setAttachement(gl::COLOR_ATTACHMENT0, output);
    RenderUtils::drawTexturedQuad(fbo, taa_tmp);
}

FrameGraph::Resource TemporalAntiAliasingPass::addPass(
    FrameGraph &frameGraph, FrameGraph::Resource input,
    FrameGraph::Resource velocity, FrameGraph::Resource depth,
    const CameraFrameData &camdata)
{
    auto inDesc = frameGraph.getTextureDesc(input);
    int width = inDesc.width;
    int height = inDesc.height;

    struct TAAPassData
    {
        FrameGraph::Resource in;
        FrameGraph::Resource velocity;
        FrameGraph::Resource depth;
        FrameGraph::Resource output;
    };
    auto &pass = frameGraph.addPass<TAAPassData>(
        //////////////////////////////////////////////////////
        // SETUP
        [&](FrameGraph::PassBuilder &builder, TAAPassData &data) {
            data.in = builder.read(input);
            data.velocity = builder.read(velocity);
            data.depth = builder.read(depth);
            data.output = builder.createTexture2D(ImageFormat::R16G16B16A16_SFLOAT,
                                                  width, height, "output");
            builder.setName("TemporalAAPass");
        },
        //////////////////////////////////////////////////////
        // EXECUTE
        [=](TAAPassData &data, FrameGraph::PassResources &pass) {
            runPass(pass.getTexture(data.in), pass.getTexture(data.velocity),
                    pass.getTexture(data.depth), pass.getTexture(data.output),
                    camdata);
            // taa_tmp -> taa_history (history buffer)
            std::swap(taa_history, taa_tmp);
        });
    return pass.output;
}
} // namespace ag