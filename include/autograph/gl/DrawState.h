#pragma once
#include <autograph/Config.h>
#include <autograph/support/Utils.h>
#include <gl_core_4_5.h>
#include <vector>


namespace ag {
namespace gl {

struct BlendState {
  constexpr BlendState() = default;
  constexpr BlendState(bool enabled_, GLenum modeRGB_, GLenum modeAlpha_,
                       GLenum funcSrcRGB_, GLenum funcDstRGB_,
                       GLenum funcSrcAlpha_, GLenum funcDstAlpha_)
      : enabled{enabled_}, modeRGB{modeRGB_}, modeAlpha{modeAlpha_},
        funcSrcRGB{funcSrcRGB_}, funcDstRGB{funcDstRGB_},
        funcSrcAlpha{funcSrcAlpha_}, funcDstAlpha{funcDstAlpha_} {}

  bool enabled = true;
  GLenum modeRGB = GL_FUNC_ADD;
  GLenum modeAlpha = GL_FUNC_ADD;
  GLenum funcSrcRGB = GL_SRC_ALPHA;
  GLenum funcDstRGB = GL_ONE_MINUS_SRC_ALPHA;
  GLenum funcSrcAlpha = GL_ONE;
  GLenum funcDstAlpha = GL_ZERO;

  constexpr size_t hash() const {
    // TODO
    return 0;
  }
};

struct DepthStencilState {
  constexpr DepthStencilState() = default;
  constexpr DepthStencilState(bool depthTestEnable_, bool depthWriteEnable_,
                              bool stencilEnable_, GLenum stencilFace_,
                              GLenum stencilFunc_, GLint stencilRef_,
                              GLuint stencilMask_, GLenum stencilOpSfail_,
                              GLenum stencilOpDPFail_, GLenum stencilOpDPPass_)
      : depthTestEnable{depthTestEnable_}, depthWriteEnable{depthWriteEnable_},
        stencilEnable{stencilEnable_}, stencilFace{stencilFace_},
        stencilFunc{stencilFunc_}, stencilRef{stencilRef_},
        stencilMask{stencilMask_}, stencilOpSfail{stencilOpSfail_},
        stencilOpDPFail{stencilOpDPFail_}, stencilOpDPPass{stencilOpDPPass_} {}

  bool depthTestEnable = false;
  bool depthWriteEnable = false;
  bool stencilEnable = false;
  GLenum stencilFace = GL_FRONT_AND_BACK;
  GLenum stencilFunc = 0;
  GLint stencilRef = 0;
  GLuint stencilMask = 0xFFFFFFFF;
  GLenum stencilOpSfail = 0;
  GLenum stencilOpDPFail = 0;
  GLenum stencilOpDPPass = 0;
};

struct RasterizerState {
  constexpr RasterizerState() = default;
  constexpr RasterizerState(GLenum fillMode_) : fillMode{fillMode_} {}
  GLenum fillMode = GL_FILL;
  GLenum cullMode = GL_NONE;

};

struct ScissorRect {
  int x;
  int y;
  int width;
  int height;
};

struct Viewport 
{
  float x;
  float y;
  float w;
  float h;
};
}
}