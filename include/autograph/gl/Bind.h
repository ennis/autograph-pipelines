#pragma once
#include <autograph/Types.h>
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Capture.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Sampler.h>
#include <autograph/gl/StateGroup.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/gl_core_4_5.h>

namespace ag {
namespace gl {

namespace bind {

inline auto uniform_float(const char *name, float v) {
  return [=](StateGroup &sg) {
    int loc = glGetUniformLocation(sg.drawStates.program, name);
    if (loc != -1) {
      glProgramUniform1f(sg.drawStates.program, loc, v);
      AG_FRAME_TRACE("uniform {} = {}", name, v);
    } else {
      AG_FRAME_TRACE(
          "uniform {} not present in shader or optimized out (program={})",
          name, sg.drawStates.program);
    }
  };
}

inline auto uniform_float(int loc, float v) {
  return [=](StateGroup &sg) {
    glProgramUniform1f(sg.drawStates.program, loc, v);
    AG_FRAME_TRACE("uniform (loc {}) = {}", loc, v);
  };
}

inline auto uniform_int(const char *name, int v) {
  return [=](StateGroup &sg) {
    int loc = glGetUniformLocation(sg.drawStates.program, name);
    if (loc != -1) {
      glProgramUniform1i(sg.drawStates.program, loc, v);
      AG_FRAME_TRACE("uniform {} = {}", name, v);
    } else {
      AG_FRAME_TRACE(
          "uniform {} not present in shader or optimized out (program={})",
          name, sg.drawStates.program);
    }
  };
}

inline auto uniform_int(int loc, int v) {
  return [=](StateGroup &sg) {
    glProgramUniform1i(sg.drawStates.program, loc, v);
    AG_FRAME_TRACE("uniform (loc {}) = {}", loc, v);
  };
}

#define UNIFORM_VECN(ty, value_ty, fn)                                         \
  inline auto uniform_##ty(const char *name, value_ty v) {                     \
    return [=](StateGroup &sg) {                                               \
      int loc = glGetUniformLocation(sg.drawStates.program, name);             \
      if (loc != -1) {                                                         \
        fn(sg.drawStates.program, loc, 1, &v[0]);                              \
        AG_FRAME_TRACE("uniform {} = {}", name, v);                            \
      } else {                                                                 \
        AG_FRAME_TRACE(                                                        \
            "uniform {} not present in shader or optimized out (program={})",  \
            name, sg.drawStates.program);                                      \
      }                                                                        \
    };                                                                         \
  }                                                                            \
  inline auto uniform_##ty(int loc, value_ty v) {                              \
    return [=](StateGroup &sg) {                                               \
      fn(sg.drawStates.program, loc, 1, &v[0]);                                \
      AG_FRAME_TRACE("uniform (loc {}) = {}", loc, v);                         \
    };                                                                         \
  }

UNIFORM_VECN(vec2, vec2, glProgramUniform2fv)
UNIFORM_VECN(vec3, vec3, glProgramUniform3fv)
UNIFORM_VECN(vec4, vec4, glProgramUniform4fv)
UNIFORM_VECN(ivec2, ivec2, glProgramUniform2iv)
UNIFORM_VECN(ivec3, ivec3, glProgramUniform3iv)
UNIFORM_VECN(ivec4, ivec4, glProgramUniform4iv)
#undef UNIFORM_VECN

#define UNIFORM_MATRIX_NXN(nxn)                                                \
  inline auto uniform_mat##nxn(const char *name, const mat##nxn &v,            \
                               bool transpose = false) {                       \
    return [=](StateGroup &sg) {                                               \
      int loc = glGetUniformLocation(sg.drawStates.program, name);             \
      if (loc != -1) {                                                         \
        AG_FRAME_TRACE("uniform {} = <matrix>", name);                         \
        glProgramUniformMatrix##nxn##fv(sg.drawStates.program, loc, 1,         \
                                        transpose, &v[0][0]);                  \
      } else {                                                                 \
        AG_FRAME_TRACE(                                                        \
            "uniform {} not present in shader or optimized out (program={})",  \
            name, sg.drawStates.program);                                      \
      }                                                                        \
    };                                                                         \
  }                                                                            \
  inline auto uniform_mat##nxn(int loc, const mat##nxn &v,                     \
                               bool transpose = false) {                       \
    return [=](StateGroup &sg) {                                               \
      glProgramUniformMatrix##nxn##fv(sg.drawStates.program, loc, 1,           \
                                      transpose, &v[0][0]);                    \
      AG_FRAME_TRACE("uniform (loc {}) = <matrix>", loc);                      \
    };                                                                         \
  }

UNIFORM_MATRIX_NXN(4)
UNIFORM_MATRIX_NXN(3)
UNIFORM_MATRIX_NXN(2)
UNIFORM_MATRIX_NXN(3x4)
#undef UNIFORM_MATRIX_NXN

inline auto texture(int unit, GLuint obj, GLuint samplerObj) {
  return [unit, obj, samplerObj](StateGroup &sg) {
    AG_FRAME_TRACE("tex unit={}, obj={}, sam={}", unit, obj, samplerObj);
    sg.uniforms.textures[unit] = obj;
    sg.uniforms.samplers[unit] = samplerObj;
  };
}

inline auto texture(int unit, const Texture &tex, Sampler &sampler) {
  return
      [ unit, obj = tex.object(), sobj = sampler.object() ](StateGroup & sg) {
    AG_FRAME_TRACE("tex unit={}, obj={}, sam={}", unit, obj, sobj);
    sg.uniforms.textures[unit] = obj;
    sg.uniforms.samplers[unit] = sobj;
  };
}

inline auto image(int unit, GLuint obj) {
  return [unit, obj](StateGroup &sg) {
    AG_FRAME_TRACE("image unit={}, obj={}", unit, obj);
    sg.uniforms.images[unit] = obj;
  };
}

inline auto image(int unit, const Texture &tex) {
  return [ unit, obj = tex.object() ](StateGroup & sg) {
    AG_FRAME_TRACE("image unit={}, obj={}", unit, obj);
    sg.uniforms.images[unit] = obj;
  };
}

inline auto uniformBuffer(int slot, BufferSlice buf) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("ubo slot={}, obj={}, offset={}, size={}", slot, buf.obj,
                   buf.offset, buf.size);
    sg.uniforms.uniformBuffers[slot] = buf.obj;
    sg.uniforms.uniformBufferOffsets[slot] = buf.offset;
    sg.uniforms.uniformBufferSizes[slot] = buf.size;
  };
}

template <typename T> inline auto uniformFrameData(int slot, const T *data) {
  auto slice = uploadFrameData(data, sizeof(T));
  return [slot, slice](StateGroup &sg) {
    AG_FRAME_TRACE("ubo slot={}, obj={}, offset={}, size={}", slot, slice.obj,
                   slice.offset, slice.size);
    sg.uniforms.uniformBuffers[slot] = slice.obj;
    sg.uniforms.uniformBufferOffsets[slot] = slice.offset;
    sg.uniforms.uniformBufferSizes[slot] = slice.size;
  };
}

inline auto vertexBuffer(int slot, BufferSlice buf, int stride) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("vbo slot={}, obj={}, offset={}, stride={}", slot, buf.obj,
                   buf.offset, stride);
    sg.uniforms.vertexBuffers[slot] = buf.obj;
    sg.uniforms.vertexBufferOffsets[slot] = buf.offset;
    sg.uniforms.vertexBufferStrides[slot] = static_cast<GLsizei>(stride);
  };
}

inline auto indexBuffer(BufferSlice buf, GLenum type) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("ibo buf={}, offset={}, size={}, index type={}", buf.obj,
                   buf.offset, buf.size, type);
    sg.uniforms.indexBuffer = buf;
    sg.uniforms.indexBufferType = type;
  };
}

inline auto program(const Program &prog) {
  return [obj = prog.object()](StateGroup & sg) {
    AG_FRAME_TRACE("program {}", obj);
    sg.drawStates.program = obj;
  };
}

inline auto framebuffer(const Framebuffer &fbo) {
  return [ obj = fbo.object(), w = fbo.width(),
           h = fbo.height() ](StateGroup & sg) {
    AG_FRAME_TRACE("fbo obj={}, size={}x{}", obj, w, h);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj);
    // initialize the default viewport and scissor
    sg.drawStates.viewports[0] = gl::Viewport{0.0f, 0.0f, (float)w, (float)h};
    sg.drawStates.scissorRects[0] = gl::ScissorRect{0, 0, w, h};
  };
}

inline auto vertexArray(const VertexArray &vao) {
  return [obj = vao.object()](StateGroup & sg) {
    AG_FRAME_TRACE("vao {}", obj);
    sg.drawStates.vertexArray = obj;
  };
}

inline auto viewport(const Viewport &vp) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("vp rect={},{}:{}x{}", vp.x, vp.y, vp.w, vp.h);
    sg.drawStates.viewports[0] = vp;
  };
}

inline auto viewport(int index, const Viewport &vp) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("vp index={}, rect={},{}:{}x{}", index, vp.x, vp.y, vp.w,
                   vp.h);
    sg.drawStates.viewports[index] = vp;
  };
}

inline auto scissor(int index, const ScissorRect &s) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("scissor index={}, rect={},{}:{}x{}", index, s.x, s.y, s.w,
                   s.h);
    sg.drawStates.scissorRects[index] = s;
  };
}

inline auto scissor(int index, int left, int bottom, int width, int height) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("scissor index={}, rect={},{}:{}x{}", index, left, bottom,
                   width, height);
    sg.drawStates.scissorRects[index] =
        ScissorRect{left, bottom, width, height};
  };
}

inline auto blendState(int index, const BlendState &bs) {
  return [index, &bs](StateGroup &sg) {
    sg.drawStates.blendStates[index] = bs;
  };
}

inline auto depthStencilState(const DepthStencilState &dss) {
  return [&dss](StateGroup &sg) {
    sg.drawStates.depthStencilState = dss;
  };
}

inline auto rasterizerState(const RasterizerState &rs) {
  return [&rs](StateGroup &sg) {
    sg.drawStates.rasterizerState = rs;
  };
}
}
}
}
