#pragma once
#include <autograph/gl/StateGroup.h>
#include <autograph/gl/DrawState.h>
#include <autograph/gl/Buffer.h>
#include <gl_core_4_5.h>

namespace ag {
namespace gl {
struct GLBindContext {
  GLuint program;
};

namespace bind {

#define UNIFORM_VECN(ty, value_ty, fn)                                         \
  auto uniform_##ty(const char *name, value_ty v) {                            \
    return [=](GLBindContext &ctx) {                                           \
      int loc = glGetUniformLocation(ctx.program, v);                          \
      if (loc != -1)                                                           \
        fn(loc, 1, &v[0]);                                                     \
    };                                                                         \
  }                                                                            \
  auto uniform_##ty(int loc, value_ty v) {                                     \
    return [=](GLBindContext &ctx) { fn(loc, 1, &v[0]); };                     \
  }

UNIFORM_VECN(vec2, vec2, glUniform2fv)
UNIFORM_VECN(vec3, vec3, glUniform3fv)
UNIFORM_VECN(vec4, vec4, glUniform4fv)
UNIFORM_VECN(ivec2, ivec2, glUniform2iv)
UNIFORM_VECN(ivec3, ivec3, glUniform3iv)
UNIFORM_VECN(ivec4, ivec4, glUniform4iv)
#undef UNIFORM_VECN

#define UNIFORM_MATRIX_NXN(nxn)                                                \
  auto uniform_mat##nxn(const char *name, const mat##nxn &v,                   \
                        bool transpose = false) {                              \
    return [=](GLBindContext &ctx) {                                           \
      int loc = glGetUniformLocation(ctx.program, v);                          \
      if (loc != -1)                                                           \
        glUniformMatrix##nxn##fv(loc, 1, transpose, &v[0][0]);                 \
    };                                                                         \
  }                                                                            \
  auto uniform_mat##nxn(int loc, const mat##nxn &v, bool transpose = false) {  \
    return [=](GLBindContext &ctx) {                                           \
      glUniformMatrix##nxn##fv(loc, 1, transpose, &v[0][0]);                   \
    };                                                                         \
  }

UNIFORM_MATRIX_NXN(4x4)
UNIFORM_MATRIX_NXN(3x3)
UNIFORM_MATRIX_NXN(2x2)
UNIFORM_MATRIX_NXN(3x4)
#undef UNIFORM_MATRIX_NXN

auto texture(int unit, const Texture &tex, const Sampler &sampler) {
  return [=](GLBindContext &) {
    glBindTextureUnit(unit, tex.object());
    glBindSampler(unit, sampler.object());
  };
}

auto image(int unit, const Texture &tex) {
  return [ unit, obj = tex.object() ](GLBindContext &) {
    glBindImageTextures(unit, 1, &obj);
  };
}

auto vertexBuffer(int slot, BufferSlice buf, int stride) {
  return [=](GLBindContext & ctx) { 
  	glBindVertexBuffer(slot, buf.obj, buf.offset,
                     static_cast<GLsizei>(stride)); 
  };
}

auto program(const Program &prog) {
  return [obj = prog.object()](GLBindContext & ctx) { glUseProgram(obj); };
}

auto framebuffer(const Framebuffer &fbo) {
  return [obj = fbo.object(), w=fbo.width(), h=fbo.height()](GLBindContext & ctx) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj);
  	glViewport(0, 0, w, h);
  };
}

auto vertexArray(const VertexArray &vao) {
  return [obj = vao.object()](GLBindContext & ctx) { glBindVertexArray(obj); };
}

auto viewport(const Viewport &vp) {
  return
      [=](GLBindContext &) { glViewportIndexedf(0, vp.x, vp.y, vp.w, vp.h); };
}

auto viewport(int index, const Viewport &vp) {
  return [=](GLBindContext &) {
    glViewportIndexedf(index, vp.x, vp.y, vp.w, vp.h);
  };
}

auto blendState(const BlendState &bs) {
  return [=](GLBindContext &) {
    glEnable(GL_BLEND); // XXX is this necessary
    if (bs.enabled) {
      glEnablei(GL_BLEND, 0);
      glBlendEquationSeparatei(0, bs.modeRGB, bs.modeAlpha);
      glBlendFuncSeparatei(0, bs.funcSrcRGB, bs.funcDstRGB, bs.funcSrcAlpha,
                           bs.funcDstAlpha);
    } else
      glDisablei(GL_BLEND, 0);
  };
}

}
}
}
