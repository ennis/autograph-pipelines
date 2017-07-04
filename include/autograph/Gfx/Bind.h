#pragma once
#include <autograph/Core/Types.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Framebuffer.h>
#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/Program.h>
#include <autograph/Gfx/Sampler.h>
#include <autograph/Gfx/StateGroup.h>
#include <autograph/Gfx/Texture.h>
#include <autograph/Gfx/VertexArray.h>
#include <autograph/Gfx/gl_core_4_5.h>

namespace ag {
namespace bind {

inline auto uniform_float(const char *name, float v) {
  return [=](StateGroup &sg) {
    int loc = gl::GetUniformLocation(sg.program, name);
    if (loc != -1) {
      gl::ProgramUniform1f(sg.program, loc, v);
      AG_FRAME_TRACE("uniform {} = {}", name, v);
    } else {
      AG_FRAME_TRACE(
          "uniform {} not present in shader or optimized out (program={})",
          name, sg.program);
    }
  };
}

inline auto uniform_float(int loc, float v) {
  return [=](StateGroup &sg) {
    gl::ProgramUniform1f(sg.program, loc, v);
    AG_FRAME_TRACE("uniform (loc {}) = {}", loc, v);
  };
}

inline auto uniform_int(const char *name, int v) {
  return [=](StateGroup &sg) {
    int loc = gl::GetUniformLocation(sg.program, name);
    if (loc != -1) {
      gl::ProgramUniform1i(sg.program, loc, v);
      AG_FRAME_TRACE("uniform {} = {}", name, v);
    } else {
      AG_FRAME_TRACE(
          "uniform {} not present in shader or optimized out (program={})",
          name, sg.program);
    }
  };
}

inline auto uniform_int(int loc, int v) {
  return [=](StateGroup &sg) {
    gl::ProgramUniform1i(sg.program, loc, v);
    AG_FRAME_TRACE("uniform (loc {}) = {}", loc, v);
  };
}

#define UNIFORM_VECN(ty, value_ty, fn)                                         \
  inline auto uniform_##ty(const char *name, value_ty v) {                     \
    return [=](StateGroup &sg) {                                               \
      int loc = gl::GetUniformLocation(sg.program, name);                      \
      if (loc != -1) {                                                         \
        fn(sg.program, loc, 1, &v[0]);                                         \
        AG_FRAME_TRACE("uniform {} = {}", name, v);                            \
      } else {                                                                 \
        AG_FRAME_TRACE(                                                        \
            "uniform {} not present in shader or optimized out (program={})",  \
            name, sg.program);                                                 \
      }                                                                        \
    };                                                                         \
  }                                                                            \
  inline auto uniform_##ty(int loc, value_ty v) {                              \
    return [=](StateGroup &sg) {                                               \
      fn(sg.program, loc, 1, &v[0]);                                           \
      AG_FRAME_TRACE("uniform (loc {}) = {}", loc, v);                         \
    };                                                                         \
  }

UNIFORM_VECN(vec2, vec2, gl::ProgramUniform2fv)
UNIFORM_VECN(vec3, vec3, gl::ProgramUniform3fv)
UNIFORM_VECN(vec4, vec4, gl::ProgramUniform4fv)
UNIFORM_VECN(ivec2, ivec2, gl::ProgramUniform2iv)
UNIFORM_VECN(ivec3, ivec3, gl::ProgramUniform3iv)
UNIFORM_VECN(ivec4, ivec4, gl::ProgramUniform4iv)
#undef UNIFORM_VECN

#define UNIFORM_MATRIX_NXN(nxn)                                                \
  inline auto uniform_mat##nxn(const char *name, const mat##nxn &v,            \
                               bool transpose = false) {                       \
    return [=](StateGroup &sg) {                                               \
      int loc = gl::GetUniformLocation(sg.program, name);                      \
      if (loc != -1) {                                                         \
        AG_FRAME_TRACE("uniform {} = <matrix>", name);                         \
        gl::ProgramUniformMatrix##nxn##fv(sg.program, loc, 1, transpose,       \
                                          &v[0][0]);                           \
      } else {                                                                 \
        AG_FRAME_TRACE(                                                        \
            "uniform {} not present in shader or optimized out (program={})",  \
            name, sg.program);                                                 \
      }                                                                        \
    };                                                                         \
  }                                                                            \
  inline auto uniform_mat##nxn(int loc, const mat##nxn &v,                     \
                               bool transpose = false) {                       \
    return [=](StateGroup &sg) {                                               \
      gl::ProgramUniformMatrix##nxn##fv(sg.program, loc, 1, transpose,         \
                                        &v[0][0]);                             \
      AG_FRAME_TRACE("uniform (loc {}) = <matrix>", loc);                      \
    };                                                                         \
  }

UNIFORM_MATRIX_NXN(4)
UNIFORM_MATRIX_NXN(3)
UNIFORM_MATRIX_NXN(2)
UNIFORM_MATRIX_NXN(3x4)
#undef UNIFORM_MATRIX_NXN

inline auto texture(int unit, gl::GLuint obj, gl::GLuint samplerObj) {
  return [unit, obj, samplerObj](StateGroup &sg) {
    AG_FRAME_TRACE("tex unit={}, obj={}, sam={}", unit, obj, samplerObj);
    sg.uniforms.textures[unit] = obj;
    sg.uniforms.samplers[unit] = samplerObj;
  };
}

inline auto texture(int unit, const Texture &tex, const Sampler &sampler) {
  return
      [ unit, obj = tex.object(), sobj = sampler.object() ](StateGroup & sg) {
    AG_FRAME_TRACE("tex unit={}, obj={}, sam={}", unit, obj, sobj);
    sg.uniforms.textures[unit] = obj;
    sg.uniforms.samplers[unit] = sobj;
  };
}

inline auto image(int unit, gl::GLuint obj) {
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

inline auto uniformBuffer(int slot, Buffer::Slice buf) {
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

inline auto vertexBuffer(int slot, Buffer::Slice buf, int stride) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("vbo slot={}, obj={}, offset={}, stride={}", slot, buf.obj,
                   buf.offset, stride);
    sg.uniforms.vertexBuffers[slot] = buf.obj;
    sg.uniforms.vertexBufferOffsets[slot] = buf.offset;
    sg.uniforms.vertexBufferStrides[slot] = static_cast<gl::GLsizei>(stride);
  };
}

inline auto indexBuffer(Buffer::Slice buf, gl::GLenum type) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("ibo buf={}, offset={}, size={}, index type={}", buf.obj,
                   buf.offset, buf.size, type);
    sg.uniforms.indexBuffer = buf;
    sg.uniforms.indexBufferType = type;
  };
}

inline auto program(const ProgramObject &prog) {
  return [obj = prog.object()](StateGroup & sg) {
    AG_FRAME_TRACE("program {}", obj);
    sg.program = obj;
  };
}

inline auto framebuffer(const Framebuffer &fbo) {
  return [ obj = fbo.object(), w = fbo.width(),
           h = fbo.height() ](StateGroup & sg) {
    AG_FRAME_TRACE("fbo obj={}, size={}x{}", obj, w, h);
    gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, obj);
    // initialize the default viewport and scissor
    sg.viewports[0] = Viewport{0.0f, 0.0f, (float)w, (float)h};
    sg.scissorRects[0] = ScissorRect{0, 0, w, h};
  };
}

inline auto vertexArray(const VertexArray &vao) {
  return [obj = vao.object()](StateGroup & sg) {
    AG_FRAME_TRACE("vao {}", obj);
    sg.vertexArray = obj;
  };
}

inline auto viewport(const Viewport &vp) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("vp rect={},{}:{}x{}", vp.x, vp.y, vp.w, vp.h);
    sg.viewports[0] = vp;
  };
}

inline auto viewport(int index, const Viewport &vp) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("vp index={}, rect={},{}:{}x{}", index, vp.x, vp.y, vp.w,
                   vp.h);
    sg.viewports[index] = vp;
  };
}

inline auto scissor(int index, const ScissorRect &s) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("scissor index={}, rect={},{}:{}x{}", index, s.x, s.y, s.w,
                   s.h);
    sg.scissorRects[index] = s;
  };
}

inline auto scissor(int index, int left, int bottom, int width, int height) {
  return [=](StateGroup &sg) {
    AG_FRAME_TRACE("scissor index={}, rect={},{}:{}x{}", index, left, bottom,
                   width, height);
    sg.scissorRects[index] = ScissorRect{left, bottom, width, height};
  };
}

inline auto blendState(int index, const BlendState &bs) {
  return [index, &bs](StateGroup &sg) { sg.blendStates[index] = bs; };
}

inline auto depthStencilState(const DepthStencilState &dss) {
  return [&dss](StateGroup &sg) { sg.depthStencilState = dss; };
}

inline auto rasterizerState(const RasterizerState &rs) {
  return [&rs](StateGroup &sg) { sg.rasterizerState = rs; };
}
} // namespace bind
} // namespace ag
