#pragma once
#include <autograph/Types.h>
#include <autograph/gl/Buffer.h>
#include <autograph/gl/DrawState.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Sampler.h>
#include <autograph/gl/StateGroup.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/VertexArray.h>
#include <gl_core_4_5.h>

namespace ag {
namespace gl {

namespace bind {

#define UNIFORM_VECN(ty, value_ty, fn)                                         \
  inline auto uniform_##ty(const char *name, value_ty v) {                            \
    return [=](StateGroup &sg) {                                               \
      int loc = glGetUniformLocation(sg.drawStates.program, name);             \
      if (loc != -1)                                                           \
        fn(loc, 1, &v[0]);                                                     \
    };                                                                         \
  }                                                                            \
  inline auto uniform_##ty(int loc, value_ty v) {                                     \
    return [=](StateGroup &sg) { fn(loc, 1, &v[0]); };                         \
  }

UNIFORM_VECN(vec2, vec2, glUniform2fv)
UNIFORM_VECN(vec3, vec3, glUniform3fv)
UNIFORM_VECN(vec4, vec4, glUniform4fv)
UNIFORM_VECN(ivec2, ivec2, glUniform2iv)
UNIFORM_VECN(ivec3, ivec3, glUniform3iv)
UNIFORM_VECN(ivec4, ivec4, glUniform4iv)
#undef UNIFORM_VECN

#define UNIFORM_MATRIX_NXN(nxn)                                                \
  inline auto uniform_mat##nxn(const char *name, const mat##nxn &v,                   \
                        bool transpose = false) {                              \
    return [=](StateGroup &sg) {                                               \
      int loc = glGetUniformLocation(sg.drawStates.program, name);             \
      if (loc != -1)                                                           \
        glUniformMatrix##nxn##fv(loc, 1, transpose, &v[0][0]);                 \
    };                                                                         \
  }                                                                            \
  inline auto uniform_mat##nxn(int loc, const mat##nxn &v, bool transpose = false) {  \
    return [=](StateGroup &sg) {                                               \
      glUniformMatrix##nxn##fv(loc, 1, transpose, &v[0][0]);                   \
    };                                                                         \
  }

UNIFORM_MATRIX_NXN(4)
UNIFORM_MATRIX_NXN(3)
UNIFORM_MATRIX_NXN(2)
UNIFORM_MATRIX_NXN(3x4)
#undef UNIFORM_MATRIX_NXN

inline auto texture(int unit, const Texture &tex, Sampler &sampler) {
  return
      [ unit, obj = tex.object(), sobj = sampler.object() ](StateGroup & sg) {
    sg.uniforms.textures[unit] = obj;
    sg.uniforms.samplers[unit] = sobj;
  };
}

inline auto image(int unit, const Texture &tex) {
  return [ unit, obj = tex.object() ](StateGroup & sg) {
    sg.uniforms.images[unit] = obj;
  };
}

inline auto uniformBuffer(int slot, BufferSlice buf) {
	return [=](StateGroup &sg) {
		sg.uniforms.uniformBuffers[slot] = buf.obj;
		sg.uniforms.uniformBufferOffsets[slot] = buf.offset;
		sg.uniforms.uniformBufferSizes[slot] = buf.size;
	};
}

inline auto vertexBuffer(int slot, BufferSlice buf, int stride) {
  return [=](StateGroup &sg) {
    sg.uniforms.vertexBuffers[slot] = buf.obj;
    sg.uniforms.vertexBufferOffsets[slot] = buf.offset;
    sg.uniforms.vertexBufferStrides[slot] = static_cast<GLsizei>(stride);
  };
}

inline auto indexBuffer(BufferSlice buf, GLenum type) {
	return [=](StateGroup &sg) {
		sg.uniforms.indexBuffer = buf;
		sg.uniforms.indexBufferType = type;
	};
}

inline auto program(const Program &prog) {
  return [obj = prog.object()](StateGroup & sg) {
    sg.drawStates.program = obj;
  };
}

inline auto framebuffer(const Framebuffer &fbo) {
  return [ obj = fbo.object(), w = fbo.width(), h = fbo.height() ](StateGroup &
                                                                   sg) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj);
    // initialize the default viewport
    sg.drawStates.viewports[0] = gl::Viewport{0.0f, 0.0f, (float)w, (float)h};
  };
}

inline auto vertexArray(const VertexArray &vao) {
  return [obj = vao.object()](StateGroup & sg) {
    sg.drawStates.vertexArray = obj;
  };
}

inline auto viewport(const Viewport &vp) {
  return [=](StateGroup &sg) { sg.drawStates.viewports[0] = vp; };
}

inline auto viewport(int index, const Viewport &vp) {
  return [=](StateGroup &sg) { sg.drawStates.viewports[index] = vp; };
}

inline auto blendState(int index, const BlendState &bs) {
  return
      [index, &bs](StateGroup &sg) { sg.drawStates.blendStates[index] = bs; };
}

inline auto depthStencilState(int index, const DepthStencilState &dss) {
  return
      [index, &dss](StateGroup &sg) { sg.drawStates.depthStencilState = dss; };
}

inline auto rasterizerState(int index, const RasterizerState &rs) {
  return [index, &rs](StateGroup &sg) { sg.drawStates.rasterizerState = rs; };
}
}
}
}
