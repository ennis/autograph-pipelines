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
protected:
  void bindTextureInternal(Pass& pass, int slot, GLuint texobj);
  void bindTextureImageInternal(Pass& pass, int slot, GLuint texobj);
  void bindSamplerInternal(Pass& pass, int slot, GLuint samplerobj);
  void bindUniformBufferInternal(Pass& pass, int slot, const ag::gl::BufferSlice &slice);
  void bindShaderStorageBufferInternal(Pass& pass, int slot, const ag::gl::BufferSlice &slice);
  void addDependencyInternal(Pass& pass, Pass &dependency);
};

class DrawPassBuilder : public PassBuilder
{
public:
  DrawPassBuilder();
  void bindTexture(int slot, GLuint texobj) { bindTextureInternal(drawPass_, slot, texobj); }
  void bindTextureImage(int slot, GLuint texobj) { bindTextureImageInternal(drawPass_, slot, texobj); }
  void bindSampler(int slot, GLuint samplerobj) { bindSamplerInternal(drawPass_, slot, samplerobj); }
  void bindUniformBuffer(int slot, const ag::gl::BufferSlice &slice) { bindUniformBufferInternal(drawPass_, slot, slice); }
  void bindShaderStorageBuffer(int slot, const ag::gl::BufferSlice &slice) { bindShaderStorageBufferInternal(drawPass_, slot, slice); }
  void addDependency(Pass &dependency) { addDependencyInternal(drawPass_, dependency); }
  void bindVertexArray(GLuint vao);
  void bindColorBuffer(int index, GLuint texobj);
  void bindDepthBuffer(GLuint texobj);
  void bindVertexBuffer(int slot, const ag::gl::BufferSlice &slice, int stride);
  void setVertexShader(std::string vs);
  void setFragmentShader(std::string fs);
  void addShaderKeyword(std::string kw);
  void addShaderDef(std::string kw, std::string def);
  void setViewport(int index, float x, float y, float width, float height);
  void setBlendState(int index, const gl::BlendState& blendState);
  void setRasterizerState(const gl::RasterizerState& rs);
  void setDepthStencilState(const gl::DepthStencilState& ds);
  auto makeDrawPass() -> std::unique_ptr<DrawPass>;

private:
  DrawPass drawPass_;
};

//
// Use that to load a standalone draw pass file without needing a full Lua context 
class DrawPassFile 
{
public:
  DrawPassFile(const char* filepath);
  auto makePass() -> std::unique_ptr<DrawPass>;
private:
}

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