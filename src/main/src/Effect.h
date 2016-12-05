#pragma once
#include <autograph/gl/Buffer.h>
#include <autograph/gl/DrawState.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Sampler.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/support/Optional.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Variant.h>
#include <memory>
#include <sol/state.hpp>
#include <vector>

struct lua_State;

namespace ag {
enum class PassType { Compute = 0, Screen, Geometry };

static constexpr int kMaxTextureUnits = 16;
static constexpr int kMaxImageUnits = 8;
static constexpr int kMaxVertexBufferSlots = 8;
static constexpr int kMaxUniformBufferSlots = 8;
static constexpr int kMaxShaderStorageBufferSlots = 8;

using UniformValue = variant<float, vec2, vec3, vec4, int, ivec2, ivec3, ivec4,
                             mat2, mat3, mat4, mat3x4>;

struct NamedUniform {
  std::string name;
  int location;
  UniformValue value;
};

struct ShaderResources {
  std::array<GLuint, kMaxTextureUnits> textures;
  std::array<GLuint, kMaxTextureUnits> samplers;
  std::array<GLuint, kMaxImageUnits> images;
  std::array<GLuint, kMaxUniformBufferSlots> uniformBuffers;
  std::array<GLsizeiptr, kMaxUniformBufferSlots> uniformBufferSizes;
  std::array<GLintptr, kMaxUniformBufferSlots> uniformBufferOffsets;
  std::array<GLuint, kMaxShaderStorageBufferSlots> shaderStorageBuffers;
  std::array<GLsizeiptr, kMaxShaderStorageBufferSlots> shaderStorageBufferSizes;
  std::array<GLintptr, kMaxShaderStorageBufferSlots> shaderStorageBufferOffsets;
  std::vector<NamedUniform> namedUniforms;
};

struct ShaderDrawResources {
  GLuint vao;
  std::array<GLuint, kMaxVertexBufferSlots> vertexBuffers;
  std::array<GLintptr, kMaxVertexBufferSlots> vertexBufferOffsets;
  std::array<GLsizei, kMaxVertexBufferSlots> vertexBufferStrides;
  GLuint indexBuffer;
  GLenum indexBufferType;
};

using Parameter =
    variant<float, vec2, vec3, vec4, int, ivec2, ivec3, ivec4, mat2, mat3, mat4,
            mat3x4, gl::Texture *, gl::BufferSlice, gl::Sampler *>;

////////////////////////////////////////////////////////
class Pass {
public:
  friend class PassBuilder;

protected:
  void compile();
  // Dependencies
  SmallVector<Pass *, 8> dependencies_;
  // gl program object
  gl::Program prog_;
  ShaderResources resources_;
};

////////////////////////////////////////////////////////
class DrawPass : public Pass {
public:
  friend class DrawPassBuilder;

private:
  void compile();
  std::string VS_;
  std::string FS_;
  gl::RasterizerState rasterizerState_;
  gl::DepthStencilState depthStencilState_;
  std::array<gl::BlendState, 8> blendStates_;
  std::array<gl::Viewport, 8> viewports_;
  std::array<GLuint, 8> colorBuffers_;
  GLuint depthBuffer_;
  // empty -> Framebuffer is specified in the draw call
  optional<gl::Framebuffer> fbo_;
  ShaderDrawResources drawResources_;
};

class PassBuilder {
public:
  void bindTexture(int slot, GLuint texobj);
  void bindTextureImage(int slot, GLuint texobj);
  void bindSampler(int slot, GLuint samplerobj);
  void bindUniformBuffer(int slot, const ag::gl::BufferSlice &slice);
  void bindShaderStorageBuffer(int slot, const ag::gl::BufferSlice &slice);
  void addDependency(ag::Pass *dependency);

protected:
  std::unique_ptr<Pass> pass_;
};

class DrawPassBuilder : public PassBuilder
{
public:
  void bindVertexArray(GLuint vao);
  void bindColorBuffer(int index, GLuint texobj);
  void bindDepthBuffer(GLuint texobj);
  void bindVertexBuffer(int slot, const ag::gl::BufferSlice &slice, int stride);

  void setVertexShader(std::string vs);
  void setFragmentShader(std::string fs);
  void addShaderKeyword(std::string kw);
  void addShaderDef(std::string kw, std::string def);

  auto makeDrawPass() -> std::unique_ptr<DrawPass>;

  auto blendState(int index) -> gl::BlendState & {
    return getPassPtr()->blendStates_[index];
  }
  auto viewport(int index) -> gl::Viewport & {
    return getPassPtr()->viewports_[index];
  }
  auto rasterizerState() -> gl::RasterizerState & {
    return getPassPtr()->rasterizerState_;
  }
  auto depthStencilState() -> gl::DepthStencilState & {
    return getPassPtr()->depthStencilState_;
  }

private:
  std::string VS_;
  std::string FS_;
  DrawPass *getPassPtr() { return static_cast<DrawPass *>(pass_.get()); }
};

////////////////////////////////////////////////////////
class ScreenPass : public Pass {
public:
private:
};

////////////////////////////////////////////////////////
class ComputePass : public Pass {
public:
private:
  optional<int> group_size_x;
  optional<int> group_size_y;
  optional<int> group_size_z;
};

}
