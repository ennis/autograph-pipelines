#ifndef DRAW_HPP
#define DRAW_HPP
#include <tuple>
#include "bind.hpp"
#include "program.hpp"

namespace ag 
{

////////////////////////// Draw command: DrawArrays
struct draw_arrays {
  GLenum primitiveType;
  uint32_t first;
  uint32_t count;

  void draw(shader_resource_bind_context& ctx)
  {
    glDrawArrays(primitiveType, first, count);
  }
};

////////////////////////// Draw command: DrawIndexed 
struct draw_indexed {
  GLenum primitiveType;
  uint32_t first;
  uint32_t count;
  uint32_t baseVertex;
  buffer_slice index_buffer;
  GLenum index_type;

  void draw(shader_resource_bind_context& ctx)
  {
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.obj);
    auto indexStride = (index_type == GL_UNSIGNED_SHORT) ? 2 : 4;
    glDrawElementsBaseVertex(
      primitiveType, count, index_type,
      ((const char*)((uintptr_t)first * indexStride)), baseVertex);
  }
};


////////////////////////// draw (no resources)
template <typename Drawable>
void draw(framebuffer& fbo,
          program& prog, 
        Drawable&& drawable) {
        shader_resource_bind_context context;
        bind_framebuffer(fbo);
  glUseProgram(prog.object());
  context.state.program = prog.object();
  drawable.draw(context);
}

////////////////////////// draw
template <typename Framebuffer, typename Drawable, typename... ShaderResources>
void draw(Framebuffer&& fbo,
          program& prog,
          Drawable&& drawable,
          ShaderResources&&... resources)
{
  shader_resource_bind_context context;
  // bind program before binding the resources (for glUniform)
  glUseProgram(prog.object());
  context.state.program = prog.object();
  bind_framebuffer(fbo);
  bind_shader_resources_rec(context, resources...);
  drawable.draw(context);
}

inline void clear(framebuffer& fb, const glm::vec4& color) {
  glClearNamedFramebufferfv(fb.object(), GL_COLOR, 0, &color[0]);
}

inline void clear(texture& tex, const glm::vec4& color) {
  glClearTexImage(tex.object(), 0, GL_RGBA, GL_FLOAT, &color[0]);
}

inline void clear_depth(framebuffer& fb,
                float depth) {
  glClearNamedFramebufferfv(fb.object(), GL_DEPTH, 0, &depth);
}

inline void clear_depth(texture& tex, float depth) {
  glClearTexImage(tex.object(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
}

inline void clear_integer(texture& tex, const glm::ivec4& color) {
  glClearTexImage(tex.object(), 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
	  &color[0]);
}
}

#endif // !DRAW_HPP
