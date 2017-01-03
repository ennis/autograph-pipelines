#include <autograph/engine/RenderUtils.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>

namespace ag {
RenderUtils::RenderUtils() {
  reloadShaders();
  gl::SamplerDesc desc;
  desc.addrU = GL_CLAMP_TO_EDGE;
  desc.addrV = GL_CLAMP_TO_EDGE;
  desc.addrW = GL_CLAMP_TO_EDGE;
  desc.minFilter = GL_NEAREST;
  desc.magFilter = GL_NEAREST;
  samplerNearest = gl::Sampler{desc};
}

void RenderUtils::reloadShaders() {
	drawSpritePass = DrawPass{ "shaders/drawSprite:drawSprite" };
}

void RenderUtils::drawSprite(gl::Framebuffer &target, float targetX0,
                             float targetY0, float targetX1, float targetY1,
                             gl::Texture &src, float srcX0, float srcY0,
                             float srcX1, float srcY1) {
  struct Vertex2D {
    float x;
    float y;
    float tx;
    float ty;
  };

  Vertex2D quad[6] = {
      {2.0f * targetX0 - 1.0f, 2.0f * targetY0 - 1.0f, srcX0, srcY0},
      {2.0f * targetX1 - 1.0f, 2.0f * targetY0 - 1.0f, srcX1, srcY0},
      {2.0f * targetX0 - 1.0f, 2.0f * targetY1 - 1.0f, srcX0, srcY1},
      {2.0f * targetX0 - 1.0f, 2.0f * targetY1 - 1.0f, srcX0, srcY1},
      {2.0f * targetX1 - 1.0f, 2.0f * targetY0 - 1.0f, srcX1, srcY0},
      {2.0f * targetX1 - 1.0f, 2.0f * targetY1 - 1.0f, srcX1, srcY1}};

  auto vbuf = ag::gl::uploadFrameArray(quad);

  using namespace gl;
  using namespace gl::bind;

  draw(target, drawArrays(GL_TRIANGLES, 0, 6), drawSpritePass.getDrawStates(),
       texture(0, src, samplerNearest),
       vertexBuffer(0, vbuf, sizeof(Vertex2D)));
}
}