#ifndef BIND_HPP
#define BIND_HPP
#include "gl_buffer.hpp"
#include "gl_device.hpp"
#include "gl_framebuffer.hpp"
#include "gl_limits.hpp"
#include "gl_pipeline.hpp"
#include "gl_sampler.hpp"
#include "gl_shader_resources.hpp"
#include "gl_texture.hpp"
#include "utils.hpp"
#include <tuple>

////////////////////////// BindContext
struct gl_bind_context {
  unsigned renderTargetBindingIndex = 0;
  unsigned textureBindingIndex = 0;
  unsigned samplerBindingIndex = 0;
  unsigned vertexBufferBindingIndex = 0;
  unsigned uniformBufferBindingIndex = 0;
  unsigned RWTextureBindingIndex = 0;
  gl_shader_resources res;
};

////////////////////////// Binder: vertex buffer
template <typename VertexTy> struct gl_vertex_buffer { const gl_buffer &buf; };

////////////////////////// Binder: vertex array (immediate vertex data)
template <typename VertexTy> struct vertex_array {
  const VertexTy *data;
  size_t size;
};

////////////////////////// Binder: index buffer
template <typename T> struct gl_index_buffer { const gl_buffer &buf; };

////////////////////////// Binder: texture unit
struct gl_texture_unit {
  gl_texture_unit(unsigned unit_, gl_texture &tex_, gl_sampler &sampler_)
      : unit(unit_), tex(tex_), sampler(sampler_) {}

  unsigned unit;
  gl_texture &tex;
  gl_sampler &sampler;
};

struct gl_image_unit {
  gl_image_unit(unsigned unit_, gl_texture &tex_) : unit(unit_), tex(tex_) {}

  unsigned unit;
  gl_texture &tex;
};

template <typename ResTy> struct gl_uniform_ {
  gl_uniform_(unsigned slot_, const ResTy &buf_) : slot(slot_), buf(buf_) {}

  unsigned slot;
  const ResTy &buf;
};

template <typename ResTy>
uniform_<ResTy> gl_uniform(unsigned slot_, const ResTy &buf_) {
  return gl_uniform_<ResTy>(slot_, buf_);
}

inline void gl_bind_surface(gl_framebuffer &fb) {
  gl::BindFramebuffer(gl::FRAMEBUFFER, fb.obj_.get());
  gl::Viewport(0, 0, fb.width, fb.height);
}

template <typename T>
void gl_bind_shader_resource(gl_bind_context &context,
                             const vertex_buffer<T> &vbuf) {
  context.res.bind_vertex_buffer(context.vertexBufferBindingIndex++,
                                 vbuf.buf.obj_.get(), 0, vbuf.buf.byte_size_,
                                 sizeof(T));
}

template <typename T>
void gl_bind_shader_resource(gl_bind_context &context,
                             const vertex_array<T> &va) {
  auto slice =
      g_device->upload_frame_data(va.data, va.size * sizeof(T), sizeof(T));
  context.res.bind_vertex_buffer(context.vertexBufferBindingIndex++, slice.obj,
                                 slice.offset, slice.size, sizeof(T));
}

template <typename T>
void gl_bind_shader_resource(gl_bind_context &context,
                             const index_buffer<T> &ibuf) {
  GLenum type;
  if (std::is_same<T, unsigned short>::value)
    type = gl::UNSIGNED_SHORT;
  if (std::is_same<T, unsigned int>::value)
    type = gl::UNSIGNED_INT;
  context.res.bind_index_buffer(ibuf.buf.obj_.get(), 0, ibuf.buf.byte_size_,
                                type);
}

inline void gl_bind_shader_resource(gl_bind_context &context, gl_texture &tex) {
  context.res.bind_texture(context.textureBindingIndex++, tex.obj_.get());
}

inline void gl_bind_shader_resource(gl_bind_context &context,
                                    gl_sampler &sampler) {
  context.res.bind_sampler(context.samplerBindingIndex++, sampler.object());
}

inline void gl_bind_shader_resource(gl_bind_context &context,
                                    const gl_texture_unit &tex_unit) {
  context.textureBindingIndex = tex_unit.unit;
  context.samplerBindingIndex = tex_unit.unit;
  gl_bind_shader_resource(context, tex_unit.sampler);
  gl_bind_shader_resource(context, tex_unit.tex);
}

inline void bind_shader_resource(gl_bind_context &context,
                                 const gl_image_unit &img_unit) {
  context.RWTextureBindingIndex = img_unit.unit;
  context.res.bind_image(context.RWTextureBindingIndex++,
                         img_unit.tex.obj_.get());
}

inline void gl_bind_shader_resource(gl_bind_context &context,
                                    const gl_buffer_slice &buf_slice) {
  context.res.bind_uniform_buffer(context.uniformBufferBindingIndex++,
                                  buf_slice.obj, buf_slice.offset,
                                  buf_slice.size);
}

template <typename T>
void gl_bind_shader_resource(gl_bind_context &context, const T &value) {
  auto slice = g_device->upload_frame_data(
      &value, sizeof(T), g_gl_impl_limits.uniform_buffer_alignment);
  gl_bind_shader_resource(context, slice);
}

template <typename T>
void gl_bind_shader_resources_rec(gl_bind_context &context, T &&resource) {
  gl_bind_shader_resource(context, std::forward<T>(resource));
}

template <typename T, typename... Rest>
void gl_bind_shader_resources_rec(gl_bind_context &context, T &&resource,
                                  Rest &&... rest) {
  gl_bind_shader_resource(context, std::forward<T>(resource));
  gl_bind_shader_resources_rec(context, std::forward<Rest>(rest)...);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// http://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element
namespace detail {
template <int... Is> struct seq {};

template <int N, int... Is> struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <int... Is> struct gen_seq<0, Is...> : seq<Is...> {};
}

namespace detail {
template <typename T, typename F, int... Is>
void for_each(T &&t, F f, seq<Is...>) {
  auto l = {(f(std::get<Is>(t)), 0)...};
}
}

template <typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> const &t, F f) {
  detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

#endif // !BIND_HPP
