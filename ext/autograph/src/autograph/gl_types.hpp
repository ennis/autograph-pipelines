#ifndef STATE_HPP
#define STATE_HPP

#include "gl_core_4_5.hpp"

namespace ag {
struct GLSamplerState {
  bool useDefault = true;
  GLenum minFilter = gl::NEAREST;
  GLenum magFilter = gl::NEAREST;
  GLenum addrU = gl::CLAMP_TO_EDGE;
  GLenum addrV = gl::CLAMP_TO_EDGE;
  GLenum addrW = gl::CLAMP_TO_EDGE;
};

struct GLBlendState {
  constexpr GLBlendState() = default;
  constexpr GLBlendState(bool enabled_, GLenum modeRGB_, GLenum modeAlpha_,
                         GLenum funcSrcRGB_, GLenum funcDstRGB_,
                         GLenum funcSrcAlpha_, GLenum funcDstAlpha_)
      : enabled{enabled_}, modeRGB{modeRGB_}, modeAlpha{modeAlpha_},
        funcSrcRGB{funcSrcRGB_}, funcDstRGB{funcDstRGB_},
        funcSrcAlpha{funcSrcAlpha_}, funcDstAlpha{funcDstAlpha_} {}

  bool enabled = true;
  GLenum modeRGB = gl::FUNC_ADD;
  GLenum modeAlpha = gl::FUNC_ADD;
  GLenum funcSrcRGB = gl::SRC_ALPHA;
  GLenum funcDstRGB = gl::ONE_MINUS_SRC_ALPHA;
  GLenum funcSrcAlpha = gl::ONE;
  GLenum funcDstAlpha = gl::ZERO;
};

struct GLDepthStencilState {
  constexpr GLDepthStencilState() = default;
  constexpr GLDepthStencilState(bool depthTestEnable_, bool depthWriteEnable_,
                                bool stencilEnable_, GLenum stencilFace_,
                                GLenum stencilFunc_, GLint stencilRef_,
                                GLuint stencilMask_, GLenum stencilOpSfail_,
                                GLenum stencilOpDPFail_,
                                GLenum stencilOpDPPass_)
      : depthTestEnable{depthTestEnable_}, depthWriteEnable{depthWriteEnable_},
        stencilEnable{stencilEnable_}, stencilFace{stencilFace_},
        stencilFunc{stencilFunc_}, stencilRef{stencilRef_},
        stencilMask{stencilMask_}, stencilOpSfail{stencilOpSfail_},
        stencilOpDPFail{stencilOpDPFail_}, stencilOpDPPass{stencilOpDPPass_} {}

  bool depthTestEnable = false;
  bool depthWriteEnable = false;
  bool stencilEnable = false;
  GLenum stencilFace = gl::FRONT_AND_BACK;
  GLenum stencilFunc = 0;
  GLint stencilRef = 0;
  GLuint stencilMask = 0xFFFFFFFF;
  GLenum stencilOpSfail = 0;
  GLenum stencilOpDPFail = 0;
  GLenum stencilOpDPPass = 0;
};

struct GLRasterizerState {
  constexpr GLRasterizerState() = default;
  constexpr GLRasterizerState(GLenum fillMode_) : fillMode{fillMode_} {}
  GLenum fillMode = gl::FILL;
};
}

#endif // !STATE_HPP
