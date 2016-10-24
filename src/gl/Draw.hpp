#pragma once
#include <autograph/gl/Bind.h>

namespace ag 
{
namespace gl {


////////////////////////// Draw command: DrawArrays
struct DrawArrays_ {
  GLenum primitiveType;
  uint32_t first;
  uint32_t count;

  void draw(ShaderResourceBindContext& ctx)
  {
    glDrawArrays(primitiveType, first, count);
  }
};

////////////////////////// Draw command: DrawIndexed 
struct DrawIndexed_ {
  GLenum primitiveType;
  uint32_t first;
  uint32_t count;
  uint32_t baseVertex;
  BufferSlice index_buffer;
  GLenum index_type;

  void draw(ShaderResourceBindContext& ctx)
  {
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.obj);
    auto indexStride = (index_type == GL_UNSIGNED_SHORT) ? 2 : 4;
    glDrawElementsBaseVertex(
      primitiveType, count, index_type,
      ((const char*)((uintptr_t)first * indexStride)), baseVertex);
  }
};


////////////////////////// draw (no resources)

template <typename Drawable, typename... DynamicParameters>
void draw(
  Framebuffer& fbo,
  Drawable&& drawable,
  StateGroup& baseState,  // shaders+draw parameters
  DynamicParameters&&... parameters) 
{
  // 1. bind program
  // 2. bind draw state
  // 3. bind dynamic parameters (can be anything: uniforms, viewports, blend state, etc.)
}

////////////////////////// draw
template <typename Framebuffer, typename Drawable, typename... ShaderResources>
void draw(Framebuffer&& fbo,
          Program& prog,
          Drawable&& drawable,
          ShaderResources&&... resources)
{
  ShaderResourceBindContext context;
  // bind program before binding the resources (for glUniform)
  glUseProgram(prog.object());
  context.state.program = prog.object();
  bind_framebuffer(fbo);
  bindShaderResourceRec(context, resources...);
  drawable.draw(context);
}

inline void clear(Framebuffer& fb, const glm::vec4& color) {
  glClearNamedFramebufferfv(fb.object(), GL_COLOR, 0, &color[0]);
}

inline void clear(Texture& tex, const glm::vec4& color) {
  glClearTexImage(tex.object(), 0, GL_RGBA, GL_FLOAT, &color[0]);
}

inline void clear_depth(Framebuffer& fb,
                float depth) {
  glClearNamedFramebufferfv(fb.object(), GL_DEPTH, 0, &depth);
}

inline void clear_depth(Texture& tex, float depth) {
  glClearTexImage(tex.object(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
}

inline void clear_integer(Texture& tex, const glm::ivec4& color) {
  glClearTexImage(tex.object(), 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
	  &color[0]);
}
}

#endif // !DRAW_HPP
