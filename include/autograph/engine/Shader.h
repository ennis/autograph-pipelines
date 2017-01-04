#pragma once
#include <autograph/gl/Buffer.h>
#include <autograph/gl/StateGroup.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Sampler.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/engine/Application.h>
#include <autograph/engine/ScriptContext.h>
#include <autograph/support/Optional.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Variant.h>
#include <autograph/support/Span.h>
#include <memory>
#include <vector>


namespace ag {
enum class PassType { Compute = 0, Screen, Geometry };

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
	ScriptContext& ensureShaderLuaStateInitialized();
	//std::unique_ptr<DrawPass> createDrawPassInternal(const char *shaderId, sol::table table);
}

////////////////////////////////////////////////////////
class Pass {
public:
  friend class PassBuilder;

protected:
  gl::Program prog_;
};

////////////////////////////////////////////////////////
class DrawPass : public Pass {
public:
	// construct an empty draw pass
	DrawPass()
	{}

	template <typename ...Args>
	DrawPass(const char* id, Args&&... args)
	{
		auto deftable = detail::ensureShaderLuaStateInitialized().create_table_with(std::forward<Args>(args)...);
		initialize(id, deftable);
	}

	void initialize(const char *shaderId, sol::table table);

  friend class DrawPassBuilder;
  // bind draw state pass to OpenGL pipeline
  const gl::DrawStates& getDrawStates() const { return drawStates_; }

private:
	// load from file
  void compile();
  gl::VertexArray vao_;
  std::string VS_;
  std::string FS_;
  gl::DrawStates drawStates_;
  //gl::RasterizerState rasterizerState_;
  //gl::DepthStencilState depthStencilState_;
  //std::array<gl::BlendState, 8> blendStates_;
  //std::array<gl::Viewport, 8> viewports_;
  std::array<GLuint, 8> colorBuffers_;
  GLuint depthBuffer_;
  // empty -> Framebuffer is specified in the draw call
  optional<gl::Framebuffer> fbo_;
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
  // load configuration from a Lua table
  void loadFromTable(sol::table config);
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
