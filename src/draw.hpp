#ifndef DRAW_HPP
#define DRAW_HPP
#include <tuple>
#include "bind.hpp"

////////////////////////// Draw command: DrawArrays
struct draw_arrays {
  GLenum prim_type;
  uint32_t first;
  uint32_t count;

  void draw(gl_bind_context& )
  {
    gl::DrawArrays(prim_type, first, count);
  }
};

////////////////////////// Draw command: DrawIndexed 
struct draw_indexed {
  GLenum prim_type;
  uint32_t first;
  uint32_t count;
  uint32_t baseVertex;

  void draw(gl_bind_context& context) 
  {
    auto indexStride = context.res.index_type == gl::UNSIGNED_SHORT ? 2 : 4;
    gl::DrawElementsBaseVertex(
		prim_type, count, context.res.index_type,
      ((const char*)((uintptr_t)first * indexStride)), baseVertex);
  }
};


////////////////////////// draw (immediate version)
template <typename Surface, typename Drawable>
void draw(Surface&& surface,
          gl_graphics_pipeline& pp, Drawable&& drawable) 
{
  gl_bind_context context;
  bind_surface(surface);
  pp.bind();
  context.res.mask = gl_shader_resource_state_mask::all;
  context.res.bind();
  drawable.draw(context);
}

////////////////////////// draw
template <typename Surface, typename Drawable, typename... ShaderResources>
void draw(Surface&& surface,
          gl_graphics_pipeline& pp,
          Drawable&& drawable,
          ShaderResources&&... resources) 
{
  gl_bind_context context;
  bind_shader_resources_rec(context, resources...);
  bind_surface(surface);
  pp.bind();
  context.res.mask = gl_shader_resource_state_mask::all;
  context.res.bind();
  drawable.draw(context);
}

struct clear_color {
  float rgba[4];
};

struct clear_color_integer {
  uint32_t rgba[4];
};

inline void clear(gl_framebuffer& fb, const clear_color& color) {
  gl::ClearNamedFramebufferfv(fb.obj_.get(), gl::COLOR, 0, color.rgba);
}

inline void clear(gl_texture& tex, const clear_color& color) {
  gl::ClearTexImage(tex.obj_.get(), 0, gl::RGBA, gl::FLOAT, color.rgba);
}

inline void clear_depth(gl_framebuffer& fb,
                float depth) {
  gl::ClearNamedFramebufferfv(fb.obj_.get(), gl::DEPTH, 0, &depth);
}

inline void clear_depth(gl_texture& tex, float depth) {
  gl::ClearTexImage(tex.obj_.get(), 0, gl::DEPTH_COMPONENT, gl::FLOAT, &depth);
}

inline void clear_integer(gl_texture& tex, const clear_color_integer& color) {
  gl::ClearTexImage(tex.obj_.get(), 0, gl::RGBA_INTEGER, gl::UNSIGNED_INT,
                    color.rgba);
}

#endif // !DRAW_HPP
