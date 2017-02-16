#pragma once
#include <autograph/Rect.h>
#include <autograph/gl/Bind.h>
#include <autograph/gl/Capture.h>

namespace ag {
namespace gl {

inline auto drawArrays(GLenum primitiveType, uint32_t first, uint32_t count) {
  return [=](StateGroup &sg) {
    bindStateGroup(sg);
    AG_FRAME_TRACE("drawArrays primitiveType={}, first={}, count={}",
                   primitiveType, first, count);
    glDrawArrays(primitiveType, first, count);
  };
}

inline auto drawIndexed(GLenum primitiveType, uint32_t first, uint32_t count,
                        uint32_t baseVertex) {
  return [=](StateGroup &sg) {
    bindStateGroup(sg);
    auto indexStride =
        (sg.uniforms.indexBufferType == GL_UNSIGNED_SHORT) ? 2 : 4;
    AG_FRAME_TRACE("drawIndexed primitiveType={}, first={}, count={}, "
                   "baseVertex={}, indexStride={}",
                   primitiveType, first, count, baseVertex, indexStride);
    glDrawElementsBaseVertex(primitiveType, count, sg.uniforms.indexBufferType,
                             ((const char *)((uintptr_t)first * indexStride)),
                             baseVertex);
  };
}

////////////////////////// draw

template <typename DrawCommand, typename Shader, typename... Arguments>
void draw(Framebuffer &fbo, DrawCommand &&drawCommand, Shader &&shader,
          Arguments &&... args) {
  StateGroup sg;
  sg.mask = StateGroupMask::All;
  // 1. bind program & draw states (~= pipeline state)
  shader(sg);
  // 1.1. bind framebuffer
  bind::framebuffer(fbo)(sg);
  // 2. bind dynamic args
  auto dummy = {0, (args(sg), 0)...};
  // 3. call render command
  // The render command is in charge of binding the state group to the pipeline
  drawCommand(sg);
}

inline void clear(Framebuffer &fb, const vec4 &color) {
  glClearNamedFramebufferfv(fb.object(), GL_COLOR, 0, &color[0]);
}

inline void clearTexture(Texture &tex, const vec4 &color) {
  glClearTexImage(tex.object(), 0, GL_RGBA, GL_FLOAT, &color[0]);
}

inline void clearDepth(Framebuffer &fb, float depth) {
  glClearNamedFramebufferfv(fb.object(), GL_DEPTH, 0, &depth);
}

inline void clearDepthTexture(Texture &tex, float depth) {
  glClearTexImage(tex.object(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
}

inline void clearTexture(Texture &tex, const ivec4 &color) {
  glClearTexImage(tex.object(), 0, GL_RGBA_INTEGER, GL_INT, &color[0]);
}

inline void clearTexture(Texture &tex, const uvec4 &color) {
  glClearTexImage(tex.object(), 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &color[0]);
}

// Draw a screen-aligned quad
template <typename Shader, typename... Arguments>
void drawQuad(Framebuffer &fbo, Shader &&shader, Arguments &&... args) {
  struct Vertex2D {
    float x;
    float y;
    float tx;
    float ty;
  };

  static constexpr Vertex2D quad[6] = {
      {-1.0f, -1.0f, 0.0f, 0.0f}, {1.0f, -1.0f, 0.0f, 0.0f},
      {-1.0f, 1.0f, 0.0f, 0.0f},  {-1.0f, 1.0f, 0.0f, 0.0f},
      {1.0f, -1.0f, 0.0f, 0.0f},  {1.0f, 1.0f, 0.0f, 0.0f}};

  // upload vertex data each frame (who cares)
  auto vbuf = ag::gl::uploadFrameArray(quad);

  draw(fbo, drawArrays(GL_TRIANGLES, 0, 6), std::forward<Shader>(shader),
       gl::bind::vertexBuffer(0, vbuf, sizeof(Vertex2D)),
       std::forward<Arguments>(args)...);
}

// Draw a screen-aligned quad
/*template <typename Shader, typename... Arguments>
void drawRect(Framebuffer &fbo, const Rect2D &rect, Shader &&shader,
              Arguments &&... args) {
  struct Vertex2D {
    float x;
    float y;
    float tx;
    float ty;
  };

  float w = (float)fbo.width();
  float h = (float)fbo.height();

  auto tl = rect.topLeft();
  auto br = rect.bottomRight();

  Vertex2D quad[6] = {
      {2.0f * tl.x / w - 1.0f, -2.0f * tl.y / h + 1.0f, 0.0f, 0.0f},
      {2.0f * br.x / w - 1.0f, -2.0f * tl.y / h + 1.0f, 1.0f, 0.0f},
      {2.0f * tl.x / w - 1.0f, -2.0f * br.y / h + 1.0f, 0.0f, 1.0f},
      {2.0f * tl.x / w - 1.0f, -2.0f * br.y / h + 1.0f, 0.0f, 1.0f},
      {2.0f * br.x / w - 1.0f, -2.0f * tl.y / h + 1.0f, 1.0f, 0.0f},
      {2.0f * br.x / w - 1.0f, -2.0f * br.y / h + 1.0f, 1.0f, 1.0f}};

  // upload vertex data each frame (who cares)
  auto vbuf = ag::gl::uploadFrameArray(quad);

  draw(fbo, drawArrays(GL_TRIANGLES, 0, 6), std::forward<Shader>(shader),
       gl::bind::vertexBuffer(0, vbuf, sizeof(Vertex2D)),
       std::forward<Arguments>(args)...);
}*/

template <typename Shader, typename... Arguments>
void drawRect(Framebuffer &fbo, float l, float t, float r, float b, float uv_l,
              float uv_t, float uv_r, float uv_b, Shader &&shader,
              Arguments &&... args) {
  struct Vertex2D {
    float x;
    float y;
    float tx;
    float ty;
  };

  float w = (float)fbo.width();
  float h = (float)fbo.height();

  Vertex2D quad[6] = {{2.0f * l / w - 1.0f, -2.0f * t / h + 1.0f, uv_l, uv_b},
                      {2.0f * r / w - 1.0f, -2.0f * t / h + 1.0f, uv_r, uv_b},
                      {2.0f * l / w - 1.0f, -2.0f * b / h + 1.0f, uv_l, uv_t},
                      {2.0f * l / w - 1.0f, -2.0f * b / h + 1.0f, uv_l, uv_t},
                      {2.0f * r / w - 1.0f, -2.0f * t / h + 1.0f, uv_r, uv_b},
                      {2.0f * r / w - 1.0f, -2.0f * b / h + 1.0f, uv_r, uv_t}};

  // upload vertex data each frame (who cares)
  auto vbuf = ag::gl::uploadFrameArray(quad);

  draw(fbo, drawArrays(GL_TRIANGLES, 0, 6), std::forward<Shader>(shader),
       gl::bind::vertexBuffer(0, vbuf, sizeof(Vertex2D)),
       std::forward<Arguments>(args)...);
}
}
}
