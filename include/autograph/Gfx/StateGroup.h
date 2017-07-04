#pragma once
#include <array>
#include <autograph/Config.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Core/Support/Flags.h>
#include <autograph/Gfx/gl_core_4_5.h>

namespace ag {

//////////////////////////////////////////////////
enum class StateGroupMask {
  Viewports = (1 << 0),   // DONE
  Framebuffer = (1 << 1), // DONE
  ScissorRect = (1 << 2),
  BlendStates = (1 << 3),       // DONE
  RasterizerState = (1 << 4),   // DONE
  DepthStencilState = (1 << 5), // DONE
  Textures = (1 << 6),          // DONE
  Samplers = (1 << 7),
  UniformBuffers = (1 << 8),       // DONE
  ShaderStorageBuffers = (1 << 9), // DONE
  VertexArray = (1 << 10),         // DONE
  Program = (1 << 11),             // DONE
  VertexBuffers = (1 << 12),       // DONE
  IndexBuffer = (1 << 13),         // DONE
  Images = (1 << 14),              // DONE
  All = 0xFFFFFFF
};

ENUM_BIT_FLAGS_OPERATORS(StateGroupMask)

static constexpr StateGroupMask StateGroupMask_AllCompute =
    StateGroupMask::Images | StateGroupMask::Textures |
    StateGroupMask::Samplers | StateGroupMask::Program |
    StateGroupMask::UniformBuffers | StateGroupMask::ShaderStorageBuffers;

//////////////////////////////////////////////////
struct BlendState {
  constexpr BlendState() = default;
  /*constexpr BlendState(bool enabled_, gl::GLenum modeRGB_, gl::GLenum modeAlpha_,
          gl::GLenum funcSrcRGB_, gl::GLenum funcDstRGB_,
          gl::GLenum funcSrcAlpha_, gl::GLenum funcDstAlpha_)
          : enabled{ enabled_ }, modeRGB{ modeRGB_ }, modeAlpha{ modeAlpha_ },
          funcSrcRGB{ funcSrcRGB_ }, funcDstRGB{ funcDstRGB_ },
          funcSrcAlpha{ funcSrcAlpha_ }, funcDstAlpha{ funcDstAlpha_ } {}*/

  bool enabled = true;
  gl::GLenum modeRGB = gl::FUNC_ADD;
  gl::GLenum modeAlpha = gl::FUNC_ADD;
  gl::GLenum funcSrcRGB = gl::SRC_ALPHA;
  gl::GLenum funcDstRGB = gl::ONE_MINUS_SRC_ALPHA;
  gl::GLenum funcSrcAlpha = gl::ONE;
  gl::GLenum funcDstAlpha = gl::ZERO;

  constexpr size_t hash() const {
    // TODO
    return 0;
  }
};

struct DepthStencilState {
  constexpr DepthStencilState() = default;
  /*constexpr DepthStencilState(bool depthTestEnable_, bool depthWriteEnable_,
          bool stencilEnable_, gl::GLenum stencilFace_,
          gl::GLenum stencilFunc_, gl::GLint stencilRef_,
          gl::GLuint stencilMask_, gl::GLenum stencilOpSfail_,
          gl::GLenum stencilOpDPFail_, gl::GLenum stencilOpDPPass_)
          : depthTestEnable{ depthTestEnable_ }, depthWriteEnable{
     depthWriteEnable_ },
          stencilEnable{ stencilEnable_ }, stencilFace{ stencilFace_ },
          stencilFunc{ stencilFunc_ }, stencilRef{ stencilRef_ },
          stencilMask{ stencilMask_ }, stencilOpSfail{ stencilOpSfail_ },
          stencilOpDPFail{ stencilOpDPFail_ }, stencilOpDPPass{ stencilOpDPPass_
     } {}*/

  bool depthTestEnable = false;
  bool depthWriteEnable = false;
  bool stencilEnable = false;
  gl::GLenum depthTestFunc = gl::LEQUAL;
  gl::GLenum stencilFace = gl::FRONT_AND_BACK;
  gl::GLenum stencilFunc = 0;
  gl::GLint stencilRef = 0;
  gl::GLuint stencilMask = 0xFFFFFFFF;
  gl::GLenum stencilOpSfail = 0;
  gl::GLenum stencilOpDPFail = 0;
  gl::GLenum stencilOpDPPass = 0;
};

struct RasterizerState {
  constexpr RasterizerState() = default;
  constexpr RasterizerState(gl::GLenum fillMode_) : fillMode{fillMode_} {}
  gl::GLenum fillMode = gl::FILL;
  gl::GLenum cullMode = gl::NONE;
  gl::GLenum frontFace = gl::CCW;
  float depthBias = 1.0f;
  float slopeScaledDepthBias = 1.0f;
  bool depthClipEnable = false;
  bool scissorEnable = false;
};

struct ScissorRect {
  int x;
  int y;
  int w;
  int h;
};

struct Viewport {
  float x;
  float y;
  float w;
  float h;
};

static constexpr int kMaxTextureUnits = 16;
static constexpr int kMaxImageUnits = 8;
static constexpr int kMaxVertexBufferSlots = 8;
static constexpr int kMaxUniformBufferSlots = 8;
static constexpr int kMaxShaderStorageBufferSlots = 8;

struct Uniforms {
  std::array<gl::GLuint, kMaxTextureUnits> textures{{0}};
  std::array<gl::GLuint, kMaxTextureUnits> samplers{{0}};
  std::array<gl::GLuint, kMaxImageUnits> images{{0}};
  std::array<gl::GLuint, kMaxUniformBufferSlots> uniformBuffers{{0}};
  std::array<gl::GLsizeiptr, kMaxUniformBufferSlots> uniformBufferSizes{{0}};
  std::array<gl::GLintptr, kMaxUniformBufferSlots> uniformBufferOffsets{{0}};
  std::array<gl::GLuint, kMaxShaderStorageBufferSlots> shaderStorageBuffers{{0}};
  std::array<gl::GLsizeiptr, kMaxShaderStorageBufferSlots> shaderStorageBufferSizes{
      {0}};
  std::array<gl::GLintptr, kMaxShaderStorageBufferSlots> shaderStorageBufferOffsets{
      {0}};
  std::array<gl::GLuint, kMaxVertexBufferSlots> vertexBuffers{{0}};
  std::array<gl::GLintptr, kMaxVertexBufferSlots> vertexBufferOffsets{{0}};
  std::array<gl::GLsizei, kMaxVertexBufferSlots> vertexBufferStrides{{0}};
  Buffer::Slice indexBuffer{0};
  gl::GLenum indexBufferType{0};
};

//struct DrawStates {
//};

struct StateGroup {
  StateGroupMask mask;
  DepthStencilState depthStencilState;
  RasterizerState rasterizerState;
  std::array<BlendState, 8> blendStates;
  std::array<ScissorRect, 8> scissorRects{ { 0 } };
  std::array<Viewport, 8> viewports{ { 0 } };
  gl::GLuint vertexArray;
  gl::GLuint program;
  Uniforms uniforms;
  gl::GLbitfield barrierBits{0};
};

// bind a uniform state group to the opengl pipeline
AG_GFX_API void bindStateGroup(const StateGroup &sg);

// binding strategy:
// accumulate all states into a temp buffer
// then send everything in one go, all the time
// states:
// => static draw states: shader pipeline + rasterizer + blend (+ VAO ?)
// => uniforms: textures, samplers, UBOs, SSBOs, uniforms, (named uniforms?)
// => draw uniforms: vertex buffer, index buffers
// => framebuffers/render targets
}
