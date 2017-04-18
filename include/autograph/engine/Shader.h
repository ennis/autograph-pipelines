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

////////////////////////////////////////////////////////
struct PipelineState {
  uint64_t hash{0};
  std::string origShaderID;
  ShaderType shaderType{ShaderType::Draw};
  bool shouldRecompile{true};
  bool compileOk{false};
  std::string vertexShaderSource;
  std::string fragmentShaderSource;
  std::string geometryShaderSource;
  std::string tessControlShaderSource;
  std::string tessEvalShaderSource;
  std::string computeShaderSource;
  ProgramObject programObject;
  VertexArray vertexArrayObject;
  DrawStates drawStates;
  gl::GLbitfield barrierBits{0};
  int groupSizeX{0};
  int groupSizeY{0};
  int groupSizeZ{0};

  void loadFromShaderFile(const char *shaderId, sol::table table);
  void loadFromTable(sol::table config);
  void compile();
};

////////////////////////////////////////////////////////
class AG_ENGINE_API PipelineStateCache {
public:
  // Look in the cache for a compiled pipeline state matching the one given as
  // input
  // Otherwise, add this one to the cache
  std::shared_ptr<PipelineState>
  cachePipelineState(std::shared_ptr<PipelineState> ps);
  // release the specified cached pipeline
  void releasePipelineState(std::shared_ptr<PipelineState> &ps);
  // get number of cached states
  int getCachedPipelineStateCount();
  PipelineState *getCachedPipelineState(int index);
  void reloadAll();

private:
  // should be hash -> CachedPipelineState
  std::vector<std::shared_ptr<PipelineState>> states;
};

AG_ENGINE_API PipelineStateCache &getPipelineStateCache();

//////////////////////////////////////////////
// Helpers
namespace detail {
AG_ENGINE_API ScriptContext &ensureShaderLuaStateInitialized();
// std::unique_ptr<DrawPass> createDrawPassInternal(const char *shaderId,
// sol::table table);
} // namespace detail

////////////////////////////////////////////////////////
class AG_ENGINE_API Shader {
public:
  // construct an empty draw pass
  Shader() {}

  // construct from shader file and defines
  template <typename... Args> Shader(const char *id, Args &&... args) {
    auto deftable = detail::ensureShaderLuaStateInitialized().create_table_with(
        std::forward<Args>(args)...);
    initialize(id, deftable);
  }

  void operator()(StateGroup &stateGroup);
  void initialize(const char *shaderId, sol::table table);
  // void loadFromTable(sol::table config);
  void bindVertexArray(gl::GLuint vao);
  void setVertexShader(std::string vs);
  void setFragmentShader(std::string fs);
  void setComputeShader(std::string cs);
  void addShaderKeyword(std::string kw);
  void addShaderDef(std::string kw, std::string def);
  void setViewport(int index, float x, float y, float width, float height);
  void setBlendState(int index, const BlendState &blendState);
  void setRasterizerState(const RasterizerState &rs);
  void setDepthStencilState(const DepthStencilState &ds);
  auto getDrawStates() -> const DrawStates &;

protected:
  std::shared_ptr<PipelineState> cached;
};
} // namespace ag
