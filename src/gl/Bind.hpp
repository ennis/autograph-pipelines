#pragma once
#include <array>
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/DrawState.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Sampler.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/support/Utils.h>
#include <tuple>

namespace ag {
namespace gl {

// draw(
//    <command>,
//    <resources...> )
// resources:
// vertexBuffer, indexBuffer, program, vertexShader(), vertexBuffer(), 
// uniform<T>, uniformBuffer, texture(name-id, tex, sampler), image, vertexArrayObject(...), shaderStorageBuffer()
// renderTarget (color(...), depth())
//
// draw(...)
// compute(...)
// makePass(...)
// makePipeline(vertexShader, fragmentShader, ...)
// 

static constexpr size_t buffer_alignment = 64;
static constexpr size_t uniform_buffer_offset_alignment = 256;
static constexpr unsigned max_texture_units = 16;
static constexpr unsigned max_image_units = 8;
static constexpr unsigned max_vertex_buffers = 8;
static constexpr unsigned max_uniform_buffers = 8;
static constexpr unsigned max_shader_storage_buffers = 8;

struct ShaderResourceState {
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
struct ShaderResourceBindContext {
  ShaderResourceState state;
  unsigned renderTargetBindingIndex = 0;
  unsigned textureBindingIndex = 0;
  unsigned samplerBindingIndex = 0;
  unsigned vertexBufferBindingIndex = 0;
  unsigned uniformBufferBindingIndex = 0;
  unsigned RWTextureBindingIndex = 0;
};

////////////////////////// Binder: vertex buffer
struct VertexBuffer {
  int slot;
  BufferSlice buf;
  size_t stride;
};

struct VertexBuffers {
  std::initializer_list<VertexBuffer> buffers;
};

////////////////////////// Binder: index buffer
struct IndexBuffer {
  const BufferSlice &buf;
  GLenum type;
};

////////////////////////// Binder: texture unit
struct TextureUnit {
  TextureUnit(unsigned unit_, Texture &tex_, Sampler &sampler_)
      : unit(unit_), tex(tex_), sampler(sampler_) {}

  unsigned unit;
  Texture &tex;
  Sampler &sampler;
};

struct TextureUnits {
  std::initializer_list<TextureUnit> units;
};

////////////////////////// Binder: image unit
struct ImageUnit {
  image_unit(unsigned unit_, Texture &tex_) : unit(unit_), tex(tex_) {}

  unsigned unit;
  Texture &tex;
};

struct ImageUnits {
  std::initializer_list<ImageUnit> units;
};

struct BlendUnit {
  int unit;
  BlendState state;
};

struct Viewport {
  int index;
  glm::ivec4 rect;
};

////////////////////////// Binder: uniform slot
struct UniformBuffer {
  unsigned slot;
  const BufferSlice &buf;
};

struct UniformFloat {
  const char *name;
  float v;
  void bind(int loc) const { glUniform1f(loc, v); }
};

struct UniformInt {
  const char *name;
  int v;
  void bind(int loc) const { glUniform1i(loc, v); }
};

#define UNIFORM_VECN(ty, value_ty, fn)                                         \
  struct Uniform##ty {                                                         \
    const char *name;                                                          \
    value_ty v;                                                                \
    void bind(int loc) const { fn(loc, 1, &v[0]); }                            \
  };

UNIFORM_VECN(float2, Float2, glUniform2fv)
UNIFORM_VECN(float3, Float3, glUniform3fv)
UNIFORM_VECN(float4, Float4, glUniform4fv)
UNIFORM_VECN(int2, Int2, glUniform2iv)
UNIFORM_VECN(int3, Int3, glUniform3iv)
UNIFORM_VECN(int4, Int4, glUniform4iv)
#undef UNIFORM_VECN

#define UNIFORM_MATRIX_NXN(nxn)                                                \
  struct UniformMatrix##nxn {                                                  \
    UniformMatrix##nxn(const char *name_, const glm::mat##nxn &v_,             \
                       bool transpose_ = false)                                \
        : name{name_}, v{v_}, transpose{transpose_} {}                         \
    const char *name;                                                          \
    const Float##nxn &v;                                                       \
    bool transpose;                                                            \
    void bind(int loc) const {                                                 \
      glUniformMatrix##nxn##fv(loc, 1, transpose, &v[0][0]);                   \
    }                                                                          \
  };
UNIFORM_MATRIX_NXN(4x4)
UNIFORM_MATRIX_NXN(3x3)
UNIFORM_MATRIX_NXN(2x2)
UNIFORM_MATRIX_NXN(3x4)
#undef UNIFORM_MATRIX_NXN

inline void bind_framebuffer(Framebuffer &fb) {
  glBindFramebuffer(GL_FRAMEBUFFER, fb.object());
  // set default Viewport (this can be overriden)
  glViewport(0, 0, fb.width(), fb.height());
}

////////////////////////// Bind<vertex_buffer>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const VertexBuffer &vbuf) {
  glBindVertexBuffer(vbuf.slot, vbuf.buf.obj, vbuf.buf.offset,
                     static_cast<GLsizei>(vbuf.stride));
}

////////////////////////// Bind<vertex_array>
/*template <typename T>
void bindShaderResource(ShaderResourceBindContext &context,
                                                  const vertex_array<T> &va) {
  auto slice = uploadFrameData(va.data, va.size * sizeof(T), sizeof(T));
  bindShaderResource(context, vertex_buffer{va.slot, slice, sizeof(T)});
}*/

////////////////////////// Bind<texture_unit<>>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const texture_unit &tex_unit) {
  assert(tex_unit.unit < getGLImplementationLimits().max_texture_units);
  glBindTextureUnit(tex_unit.unit, tex_unit.tex.object());
  glBindSampler(tex_unit.unit, tex_unit.sampler.object());
}

////////////////////////// Bind<image_unit<>>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const image_unit &img_unit) {
  assert(img_unit.unit <
         getGLImplementationLimits().max_combined_texture_image_units);
  auto obj = img_unit.tex.object();
  glBindImageTextures(img_unit.unit, 1, &obj);
}

////////////////////////// Bind<vertex_array>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const VertexArray &vao) {
  glBindVertexArray(vao.object());
}

#define BIND_NAMED_UNIFORM(ty)                                                 \
  \
inline void                                                                    \
  bindShaderResource(ShaderResourceBindContext &context,                       \
                     const Uniform##ty &u) {                                   \
    GLuint loc = glGetUniformLocation(context.state.program, u.name);          \
    u.bind(loc);                                                               \
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

////////////////////////// Bind<Viewport>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const Viewport &v) {
  glViewport(v.rect.x, v.rect.y, v.rect.z, v.rect.w);
}

////////////////////////// Bind<BlendUnit>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const BlendUnit &bs) {
  glEnable(GL_BLEND); // XXX is this necessary
  if (bs.state.enabled) {
    glEnablei(GL_BLEND, bs.unit);
    glBlendEquationSeparatei(bs.unit, bs.state.modeRGB, bs.state.modeAlpha);
    glBlendFuncSeparatei(bs.unit, bs.state.funcSrcRGB, bs.state.funcDstRGB,
                         bs.state.funcSrcAlpha, bs.state.funcDstAlpha);
  } else
    glDisablei(GL_BLEND, bs.unit);
}

////////////////////////// Bind<RasterizerState>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const RasterizerState &rs) {
  glPolygonMode(GL_FRONT_AND_BACK, rs.fillMode);
  glDisable(GL_CULL_FACE);
}

////////////////////////// Bind<DepthStencilState>
inline void bindShaderResource(ShaderResourceBindContext &context,
                               const DepthStencilState &dss) {
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
inline void bindShaderResourceRec(ShaderResourceBindContext &context,
                                  T &&resource) {
  bindShaderResource(context, std::forward<T>(resource));
}

template <typename T, typename... Rest>
inline void bindShaderResourceRec(ShaderResourceBindContext &context,
                                  T &&resource, Rest &&... rest) {
  bindShaderResource(context, std::forward<T>(resource));
  bindShaderResourceRec(context, std::forward<Rest>(rest)...);
}
}
}