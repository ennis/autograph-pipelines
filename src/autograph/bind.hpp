#ifndef BIND_HPP
#define BIND_HPP

#include "buffer.hpp"
#include "device.hpp"
#include "draw_state.hpp"
#include "framebuffer.hpp"
#include "sampler.hpp"
#include "texture.hpp"
#include "vertex_array.hpp"
#include "utils.hpp"
#include <array>
#include <tuple>

namespace ag {

static constexpr size_t buffer_alignment = 64;
static constexpr size_t uniform_buffer_offset_alignment = 256;
static constexpr unsigned max_texture_units = 16;
static constexpr unsigned max_image_units = 8;
static constexpr unsigned max_vertex_buffers = 8;
static constexpr unsigned max_uniform_buffers = 8;
static constexpr unsigned max_shader_storage_buffers = 8;

struct shader_resource_bind_state {
  std::array<GLuint, max_vertex_buffers> vertexBuffers;
  std::array<GLintptr, max_vertex_buffers> vertexBufferOffsets;
  std::array<GLsizei, max_vertex_buffers> vertexBufferStrides;
  std::array<GLuint, max_texture_units> textures;
  std::array<GLuint, max_texture_units> samplers;
  std::array<GLuint, max_image_units> images;
  std::array<GLuint, max_uniform_buffers> uniformBuffers;
  std::array<GLsizeiptr, max_uniform_buffers> uniformBufferSizes;
  std::array<GLintptr, max_uniform_buffers> uniformBufferOffsets;
  std::array<GLuint, max_shader_storage_buffers> shaderStorageBuffers;
  GLuint indexBuffer;
  GLenum indexBufferType;
  GLuint program;
  bool isComputeCall;
  void bind();
  void reset();
};

////////////////////////// BindContext
struct shader_resource_bind_context {
  shader_resource_bind_state state;
  unsigned renderTargetBindingIndex = 0;
  unsigned textureBindingIndex = 0;
  unsigned samplerBindingIndex = 0;
  unsigned vertexBufferBindingIndex = 0;
  unsigned uniformBufferBindingIndex = 0;
  unsigned RWTextureBindingIndex = 0;
};

////////////////////////// Binder: vertex buffer
struct vertex_buffer {
  int slot;
  buffer_slice buf;
  size_t stride;
};

struct vertex_buffers {
  std::initializer_list<vertex_buffer> buffers;
};

////////////////////////// Binder: index buffer
struct index_buffer {
  const buffer_slice &buf;
  GLenum type;
};

////////////////////////// Binder: texture unit
struct texture_unit {
  texture_unit(unsigned unit_, texture &tex_, sampler &sampler_)
      : unit(unit_), tex(tex_), sampler(sampler_) {}

  unsigned unit;
  texture &tex;
  sampler &sampler;
};

struct texture_units {
  std::initializer_list<texture_unit> units;
};

////////////////////////// Binder: image unit
struct image_unit {
  image_unit(unsigned unit_, texture &tex_) : unit(unit_), tex(tex_) {}

  unsigned unit;
  texture &tex;
};

struct image_units {
  std::initializer_list<image_unit> units;
};

struct blend_unit {
  int unit;
  blend_state state;
};

struct viewport {
  int index;
  glm::ivec4 rect;
};

////////////////////////// Binder: uniform slot
struct uniform_buffer {
  unsigned slot;
  const buffer_slice &buf;
};

struct uniform_float {
  const char *name;
  float v;
  void bind(int loc) const { glUniform1f(loc, v); }
};

struct uniform_int {
  const char *name;
  int v;
  void bind(int loc) const { glUniform1i(loc, v); }
};

#define UNIFORM_VECN(ty, value_ty, fn)                                         \
  struct uniform_##ty {                                                        \
    const char *name;                                                          \
    value_ty v;                                                                \
    void bind(int loc) const { fn(loc, 1, &v[0]); }                            \
  };

UNIFORM_VECN(float2, glm::vec2, glUniform2fv)
UNIFORM_VECN(float3, glm::vec3, glUniform4fv)
UNIFORM_VECN(float4, glm::vec4, glUniform4fv)
UNIFORM_VECN(int2, glm::ivec2, glUniform2iv)
UNIFORM_VECN(int3, glm::ivec3, glUniform3iv)
UNIFORM_VECN(int4, glm::ivec4, glUniform4iv)
#undef UNIFORM_VECN

#define UNIFORM_MATRIX_NXN(nxn)                                                \
  struct uniform_matrix##nxn {                                                 \
    uniform_matrix##nxn(const char *name_, const glm::mat##nxn &v_,            \
                        bool transpose_ = false)                               \
        : name{name_}, v{v_}, transpose{transpose_} {}                         \
    const char *name;                                                          \
    const glm::mat##nxn &v;                                                    \
    bool transpose;                                                            \
    void bind(int loc) const {                                                 \
      glUniformMatrix##nxn##fv(loc, 1, transpose, &v[0][0]);                   \
    }                                                                          \
  };
UNIFORM_MATRIX_NXN(4)
UNIFORM_MATRIX_NXN(3)
UNIFORM_MATRIX_NXN(2)
UNIFORM_MATRIX_NXN(3x4)
#undef UNIFORM_MATRIX_NXN

////////////////////////// Binder: surface
inline void bind_framebuffer(GLuint fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

inline void bind_framebuffer(framebuffer &fb) {
  glBindFramebuffer(GL_FRAMEBUFFER, fb.object());
  // set default viewport (this can be overriden)
  glViewport(0, 0, fb.width(), fb.height());
}

////////////////////////// Bind<vertex_buffer>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const vertex_buffer &vbuf) {
  glBindVertexBuffer(vbuf.slot, vbuf.buf.obj, vbuf.buf.offset,
                     static_cast<GLsizei>(vbuf.stride));
}

////////////////////////// Bind<vertex_array>
/*template <typename T>
void bind_shader_resource(shader_resource_bind_context &context,
                          const vertex_array<T> &va) {
  auto slice = upload_frame_data(va.data, va.size * sizeof(T), sizeof(T));
  bind_shader_resource(context, vertex_buffer{va.slot, slice, sizeof(T)});
}*/

////////////////////////// Bind<texture_unit<>>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const texture_unit &tex_unit) {
  assert(tex_unit.slot < get_impl_limits().max_texture_units);
  glBindTextureUnit(tex_unit.unit, tex_unit.tex.object());
  glBindSampler(tex_unit.unit, tex_unit.sampler.object());
}

////////////////////////// Bind<image_unit<>>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const image_unit &img_unit) {
  assert(tex_unit.slot < get_impl_limits().max_combined_texture_image_units);
  auto obj = img_unit.tex.object();
  glBindImageTextures(img_unit.unit, 1, &obj);
}

////////////////////////// Bind<vertex_array>
inline void bind_shader_resource(shader_resource_bind_context &context,
	const vertex_array &vao) {
	glBindVertexArray(vao.object());
}


#define BIND_NAMED_UNIFORM(ty)                                                                                                          \
  \
void bind_shader_resource(shader_resource_bind_context &context,             \
                          const uniform_##ty &u) { \
    GLuint loc = glGetUniformLocation(context.state.program, u.name);                                                                   \
    u.bind(loc);                                                                                                                        \
  \
}
BIND_NAMED_UNIFORM(float)
BIND_NAMED_UNIFORM(float2)
BIND_NAMED_UNIFORM(float3)
BIND_NAMED_UNIFORM(float4)
BIND_NAMED_UNIFORM(int)
BIND_NAMED_UNIFORM(int2)
BIND_NAMED_UNIFORM(int3)
BIND_NAMED_UNIFORM(int4)
BIND_NAMED_UNIFORM(matrix4)
BIND_NAMED_UNIFORM(matrix3)
BIND_NAMED_UNIFORM(matrix2)
BIND_NAMED_UNIFORM(matrix3x4)
#undef BIND_NAMED_UNIFORM

////////////////////////// Bind<viewport>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const viewport &v) {
  glViewport(v.rect.x, v.rect.y, v.rect.z, v.rect.w);
}

////////////////////////// Bind<blend_unit>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const blend_unit &bs) {
  glEnable(GL_BLEND); // XXX is this necessary
  if (bs.state.enabled) {
    glEnablei(GL_BLEND, bs.unit);
    glBlendEquationSeparatei(bs.unit, bs.state.modeRGB, bs.state.modeAlpha);
    glBlendFuncSeparatei(bs.unit, bs.state.funcSrcRGB, bs.state.funcDstRGB,
                         bs.state.funcSrcAlpha, bs.state.funcDstAlpha);
  } else
    glDisablei(GL_BLEND, bs.unit);
}

////////////////////////// Bind<rasterizer_state>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const rasterizer_state &rs) {
  glPolygonMode(GL_FRONT_AND_BACK, rs.fillMode);
  glDisable(GL_CULL_FACE);
}

////////////////////////// Bind<depth_stencil_state>
inline void bind_shader_resource(shader_resource_bind_context &context,
                                 const depth_stencil_state &dss) {
  if (dss.depthTestEnable)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  if (dss.stencilEnable) {
    glEnable(GL_STENCIL_TEST);
    glStencilFuncSeparate(dss.stencilFace, dss.stencilFunc, dss.stencilRef,
                          dss.stencilMask);
    glStencilOp(dss.stencilOpSfail, dss.stencilOpDPFail, dss.stencilOpDPPass);
  } else
    glDisable(GL_STENCIL_TEST);
}

//////////////////////////
template <typename T>
void bind_shader_resources_rec(shader_resource_bind_context &context,
                               T &&resource) {
  bind_shader_resource(context, std::forward<T>(resource));
}

template <typename T, typename... Rest>
void bind_shader_resources_rec(shader_resource_bind_context &context,
                               T &&resource, Rest &&... rest) {
  bind_shader_resource(context, std::forward<T>(resource));
  bind_shader_resources_rec(context, std::forward<Rest>(rest)...);
}
}

#endif // !BIND_HPP
