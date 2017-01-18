#pragma once
#include <array>
#include <autograph/Config.h>
#include <autograph/gl/Buffer.h>
#include <autograph/support/Optional.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Utils.h>
#include <gl_core_4_5.h>

namespace ag {
namespace gl {

//////////////////////////////////////////////////
enum class AG_REFLECT StateGroupMask {
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

//////////////////////////////////////////////////
struct AG_API BlendState {
  constexpr BlendState() = default;
  /*constexpr BlendState(bool enabled_, GLenum modeRGB_, GLenum modeAlpha_,
          GLenum funcSrcRGB_, GLenum funcDstRGB_,
          GLenum funcSrcAlpha_, GLenum funcDstAlpha_)
          : enabled{ enabled_ }, modeRGB{ modeRGB_ }, modeAlpha{ modeAlpha_ },
          funcSrcRGB{ funcSrcRGB_ }, funcDstRGB{ funcDstRGB_ },
          funcSrcAlpha{ funcSrcAlpha_ }, funcDstAlpha{ funcDstAlpha_ } {}*/

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

struct AG_API DepthStencilState {
  constexpr DepthStencilState() = default;
  /*constexpr DepthStencilState(bool depthTestEnable_, bool depthWriteEnable_,
          bool stencilEnable_, GLenum stencilFace_,
          GLenum stencilFunc_, GLint stencilRef_,
          GLuint stencilMask_, GLenum stencilOpSfail_,
          GLenum stencilOpDPFail_, GLenum stencilOpDPPass_)
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
  GLenum depthTestFunc = GL_LEQUAL;
  GLenum stencilFace = GL_FRONT_AND_BACK;
  GLenum stencilFunc = 0;
  GLint stencilRef = 0;
  GLuint stencilMask = 0xFFFFFFFF;
  GLenum stencilOpSfail = 0;
  GLenum stencilOpDPFail = 0;
  GLenum stencilOpDPPass = 0;
};

struct AG_API RasterizerState {
  constexpr RasterizerState() = default;
  constexpr RasterizerState(GLenum fillMode_) : fillMode{fillMode_} {}
  GLenum fillMode = GL_FILL;
  GLenum cullMode = GL_NONE;
  GLenum frontFace = GL_CCW;
  float depthBias = 1.0f;
  float slopeScaledDepthBias = 1.0f;
  bool depthClipEnable = false;
  bool scissorEnable = false;
};

struct AG_API ScissorRect {
  int x;
  int y;
  int width;
  int height;
};

struct AG_API Viewport {
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

struct AG_API Uniforms {
  std::array<GLuint, kMaxTextureUnits> textures{{0}};
  std::array<GLuint, kMaxTextureUnits> samplers{{0}};
  std::array<GLuint, kMaxImageUnits> images{{0}};
  std::array<GLuint, kMaxUniformBufferSlots> uniformBuffers{{0}};
  std::array<GLsizeiptr, kMaxUniformBufferSlots> uniformBufferSizes{{0}};
  std::array<GLintptr, kMaxUniformBufferSlots> uniformBufferOffsets{{0}};
  std::array<GLuint, kMaxShaderStorageBufferSlots> shaderStorageBuffers{{0}};
  std::array<GLsizeiptr, kMaxShaderStorageBufferSlots> shaderStorageBufferSizes{
      {0}};
  std::array<GLintptr, kMaxShaderStorageBufferSlots> shaderStorageBufferOffsets{
      {0}};
  std::array<GLuint, kMaxVertexBufferSlots> vertexBuffers{{0}};
  std::array<GLintptr, kMaxVertexBufferSlots> vertexBufferOffsets{{0}};
  std::array<GLsizei, kMaxVertexBufferSlots> vertexBufferStrides{{0}};
  BufferSlice indexBuffer{0};
  GLenum indexBufferType{0};
};

struct AG_API DrawStates {
  DepthStencilState depthStencilState;
  RasterizerState rasterizerState;
  ScissorRect scissorRect;
  std::array<Viewport, 8> viewports{{0}};
  std::array<BlendState, 8> blendStates;
  GLuint vertexArray;
  GLuint program;
};

struct AG_API StateGroup {
  StateGroupMask mask;
  DrawStates drawStates;
  Uniforms uniforms;
};

// bind a uniform state group to the opengl pipeline
AG_API void bindStateGroup(const StateGroup &sg);

// binding strategy:
// accumulate all states into a temp buffer
// then send everything in one go, all the time
// states:
// => static draw states: shader pipeline + rasterizer + blend (+ VAO ?)
// => uniforms: textures, samplers, UBOs, SSBOs, uniforms, (named uniforms?)
// => draw uniforms: vertex buffer, index buffers
// => framebuffers/render targets
}
}
