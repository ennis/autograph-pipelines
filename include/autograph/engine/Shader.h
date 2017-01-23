#pragma once
#include <autograph/engine/Application.h>
#include <autograph/engine/ScriptContext.h>
#include <autograph/gl/Bind.h>
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Sampler.h>
#include <autograph/gl/StateGroup.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/support/Optional.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Span.h>
#include <autograph/support/Variant.h>
#include <memory>
#include <vector>

namespace ag {
enum class ShaderType { Compute = 0, Draw };

using UniformValue = variant<float, vec2, vec3, vec4, int, ivec2, ivec3, ivec4,
                             mat2, mat3, mat4, mat3x4>;

struct NamedUniform {
  std::string name;
  int location;
  UniformValue value;
};

using Parameter =
    variant<float, vec2, vec3, vec4, int, ivec2, ivec3, ivec4, mat2, mat3, mat4,
            mat3x4, gl::Texture *, gl::BufferSlice, gl::Sampler *>;

//////////////////////////////////////////////
// Helpers
namespace detail {
ScriptContext &ensureShaderLuaStateInitialized();
// std::unique_ptr<DrawPass> createDrawPassInternal(const char *shaderId,
// sol::table table);
}

////////////////////////////////////////////////////////
class Shader {
public:
  // construct an empty draw pass
  Shader() {}

  // construct from shader file and defines
  template <typename... Args> Shader(const char *id, Args &&... args) {
    auto deftable = detail::ensureShaderLuaStateInitialized().create_table_with(
        std::forward<Args>(args)...);
    initialize(id, deftable);
  }

  void operator()(gl::StateGroup &stateGroup);

  void initialize(const char *shaderId, sol::table table);
  void loadFromTable(sol::table config);
  // void bindTexture(int slot, GLuint texobj) { bindTextureInternal(drawPass_,
  // slot, texobj); }
  // void bindTextureImage(int slot, GLuint texobj) {
  // bindTextureImageInternal(drawPass_, slot, texobj); }
  // void bindSampler(int slot, GLuint samplerobj) {
  // bindSamplerInternal(drawPass_, slot, samplerobj); }
  // void bindUniformBuffer(int slot, const ag::gl::BufferSlice &slice) {
  // bindUniformBufferInternal(drawPass_, slot, slice); }
  // void bindShaderStorageBuffer(int slot, const ag::gl::BufferSlice &slice) {
  // bindShaderStorageBufferInternal(drawPass_, slot, slice); }
  void bindVertexArray(GLuint vao);
  void bindColorBuffer(int index, GLuint texobj);
  void bindDepthBuffer(GLuint texobj);
  // void bindVertexBuffer(int slot, const ag::gl::BufferSlice &slice, int
  // stride);
  void setVertexShader(std::string vs);
  void setFragmentShader(std::string fs);
  void setComputeShader(std::string cs);
  void addShaderKeyword(std::string kw);
  void addShaderDef(std::string kw, std::string def);
  void setViewport(int index, float x, float y, float width, float height);
  void setBlendState(int index, const gl::BlendState &blendState);
  void setRasterizerState(const gl::RasterizerState &rs);
  void setDepthStencilState(const gl::DepthStencilState &ds);
  auto getDrawStates() -> const gl::DrawStates & {
    if (shouldRecompile_) {
      compile();
    }
    return drawStates_;
  }

protected:
  ShaderType shaderType_{ShaderType::Draw};
  bool shouldRecompile_{true};
  bool compileOk_{false};
  gl::Program prog_;
  // load from file
  void compile();
  gl::VertexArray vao_;
  std::string VS_;
  std::string FS_;
  std::string GS_;
  std::string TCS_;
  std::string TES_;
  std::string CS_;
  gl::DrawStates drawStates_;
  GLbitfield barrierBits_{0};
  // gl::RasterizerState rasterizerState_;
  // gl::DepthStencilState depthStencilState_;
  // std::array<gl::BlendState, 8> blendStates_;
  // std::array<gl::Viewport, 8> viewports_;
  std::array<GLuint, 8> colorBuffers_;
  GLuint depthBuffer_{0};
  // empty -> Framebuffer is specified in the draw call
  optional<gl::Framebuffer> fbo_;
  // for compute shaders
  optional<int> groupSizeX;
  optional<int> groupSizeY;
  optional<int> groupSizeZ;
};
}
