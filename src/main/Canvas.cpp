#include "Canvas.h"
#include <glm/gtc/random.hpp>
#include <glm/gtc/packing.hpp>
#include <autograph/gl/Draw.h>
#include <autograph/gl/Device.h>


namespace ag {

/////////////////////////////////////////////////////////////
template <typename T> T evalJitter(T ref, T jitter) {
  return ref + glm::linearRand(-jitter, jitter);
}

/////////////////////////////////////////////////////////////
int BrushPathBuilder::addPoint(const Params &params, float x, float y,
                               std::function<void(Point)> f) {
  auto spacing = evalJitter(params.spacing, params.spacingJitter);
  if (spacing < 0.1f)
    spacing = 0.1f;

  if (pointerPositions_.empty()) {
    pointerPositions_.push_back(vec2{x, y});
    splatPositions_.push_back(vec2{x, y});
    f(Point{true, x, y, 0.0f, 0.0f});
    return 1;
  } else {
    auto Plast = pointerPositions_.back();
    vec2 Pmouse{x, y};
    vec2 Pcur = glm::mix(Plast, Pmouse, 0.5); // smoothing
    auto length = glm::distance(Plast, Pcur);
    auto slack = pathLength_;
    pathLength_ += length;
    auto pos = spacing - slack;
    int n = 0;
    auto Pprev = splatPositions_.back();
    while (pathLength_ > spacing) {
      auto P = glm::mix(Plast, Pcur, (length > 0.01f) ? pos / length : 0.0f);
      splatPositions_.push_back(P);
      f(Point{false, P.x, P.y, Pprev.x, Pprev.y});
      Pprev = P;
      ++n;
      pathLength_ -= spacing;
      pos += spacing;
    }
    pointerPositions_.push_back(Pmouse);
    return n;
  }
}

/////////////////////////////////////////////////////////////
void BrushPathBuilder::clear() {
  pointerPositions_.clear();
  splatPositions_.clear();
  pathLength_ = 0.0f;
}

/////////////////////////////////////////////////////////////
Canvas::Canvas(int width_, int height_) : width{ width_ }, height{ height_ } {
  // Normals (from camera)
  // Palette coeffs: need 12 coeffs (normalized uint16_t, packed in two
  // R32G32B32A32 textures)
  // Color flat reference positions: two texcoords (normalized uint16_ts)

  GBuffers =
      RenderTarget{width,
                   height,
                   {ag::ImageFormat::R16G16B16A16_SFLOAT},
                   RenderTarget::DepthTexture{ag::ImageFormat::D32_SFLOAT}};

  canvasBuffers = RenderTarget{
      width,
      height,
      {ag::ImageFormat::R32G32B32A32_UINT, ag::ImageFormat::R32G32B32A32_UINT},
      RenderTarget::NoDepth{}};

  // final color: R16G16A16B16 float
  finalColor = RenderTarget{width,
                            height,
                            {ag::ImageFormat::R16G16B16A16_SFLOAT},
                            RenderTarget::NoDepth{}};

  glm::uint a0 = glm::packUnorm2x16(vec2{ 0.5f, 0.5f });
  glm::uint a1 = glm::packUnorm2x16(vec2{ 0.5f, 0.5f });
  glm::uint a2 = glm::packUnorm2x16(vec2{ 0.5f, 0.5f });
  glm::uint a3 = glm::packUnorm2x16(vec2{ 2.0f, 1.0f });
  uvec4 A = uvec4{ a0,a1,a2,a3 };

  glm::uint b0 = glm::packUnorm2x16(vec2{ 0.0f, 0.5f });
  glm::uint b1 = glm::packUnorm2x16(vec2{ 0.2f, 0.25f });
  glm::uint b2 = glm::packUnorm2x16(vec2{ 0.5f, 0.5f });
  glm::uint b3 = glm::packUnorm2x16(vec2{ 0.5f, 0.5f });
  uvec4 B = uvec4{ b0,b1,b2,b3 };

  gl::clearTexture(canvasBuffers.getColorTarget(0), A);
  gl::clearTexture(canvasBuffers.getColorTarget(1), B);
}

CanvasRenderer::CanvasRenderer() 
{
	gl::SamplerDesc desc;
	desc.addrU = GL_CLAMP_TO_EDGE;
	desc.addrV = GL_CLAMP_TO_EDGE;
	desc.addrW = GL_CLAMP_TO_EDGE;
	desc.minFilter = GL_NEAREST;
	desc.magFilter = GL_NEAREST;
	sampler = gl::Sampler{ desc }; 
	reloadShaders(); 
}

void CanvasRenderer::renderCanvas(Scene &scene, Canvas &canvas) 
{
	struct Vertex2D {
		float x;
		float y;
		float tx;
		float ty;
	};

	Vertex2D quad[6] = {
		{ - 1.0f, - 1.0f, 0.0f, 0.0f },
		{ 1.0f, - 1.0f, 0.0f, 0.0f },
		{  - 1.0f, 1.0f, 0.0f, 0.0f },
		{  - 1.0f, 1.0f, 0.0f, 0.0f },
		{  1.0f,  - 1.0f, 0.0f, 0.0f },
		{ 1.0f,  1.0f, 0.0f, 0.0f } };

	auto vbuf = ag::gl::uploadFrameArray(quad);

	using namespace gl;
	using namespace gl::bind;

	draw(canvas.finalColor.getFramebuffer(), 
		drawArrays(GL_TRIANGLES, 0, 6), 
		evaluationPass,
		texture(0, canvas.GBuffers.getColorTarget(0), sampler),
		texture(1, canvas.canvasBuffers.getColorTarget(0), sampler),
		texture(2, canvas.canvasBuffers.getColorTarget(1), sampler),
		vertexBuffer(0, vbuf, sizeof(Vertex2D)));
}

void CanvasRenderer::reloadShaders() {
  gbufferPass = Shader{"shaders/canvas:gbuffer"};
  evaluationPass = Shader{"shaders/canvas:evaluation"};
}

}