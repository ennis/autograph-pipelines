#pragma once
#include <vector>
#include "utils.hpp"

namespace ag {

struct blend_state {
  constexpr blend_state() = default;
  constexpr blend_state(bool enabled_, GLenum modeRGB_, GLenum modeAlpha_,
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

struct depth_stencil_state {
  constexpr depth_stencil_state() = default;
  constexpr depth_stencil_state(bool depthTestEnable_, bool depthWriteEnable_,
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
  GLenum stencilFace = GL_FRONT_AND_BACK;
  GLenum stencilFunc = 0;
  GLint stencilRef = 0;
  GLuint stencilMask = 0xFFFFFFFF;
  GLenum stencilOpSfail = 0;
  GLenum stencilOpDPFail = 0;
  GLenum stencilOpDPPass = 0;
};

struct rasterizer_state {
  constexpr rasterizer_state() = default;
  constexpr rasterizer_state(GLenum fillMode_) : fillMode{fillMode_} {}
  GLenum fillMode = GL_FILL;
};

enum class draw_state_mask {
  viewports = (1 << 0),
  draw_buffers = (1 << 1),
  scissor_rect = (1 << 2),
  blend_states = (1 << 3),
  rasterizer_state = (1 << 4),
  depth_stencil_state = (1 << 5)
};

/*struct draw_state {
  GLenum prim_mode;
  draw_state_mask mask;
  std::vector<blend_state> blend_states;
  std::vector<viewport> viewports;
  rasterizer_state rasterizer_state;
  depth_stencil_state depth_stencil_state;
};*/

ENUM_BIT_FLAGS_OPERATORS(draw_state_mask)
}