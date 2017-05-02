#pragma once
#include <autograph/Engine/Application.h>
#include <autograph/Engine/ScriptContext.h>
#include <autograph/Engine/ResourcePool.h>
#include <autograph/Gfx/Bind.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Framebuffer.h>
#include <autograph/Gfx/Program.h>
#include <autograph/Gfx/Sampler.h>
#include <autograph/Gfx/StateGroup.h>
#include <autograph/Gfx/Texture.h>
#include <autograph/Gfx/VertexArray.h>
#include <autograph/Core/Support/Optional.h>
#include <autograph/Core/Support/SmallVector.h>
#include <autograph/Core/Support/Span.h>
#include <autograph/Core/Support/Variant.h>
#include <autograph/Core/Support/Flags.h>
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

struct CachedPipeline : public CacheObject
{
	uint64_t hash{ 0 };
	// Path of the shader file, if it was loaded from a file
	std::string origShaderPath;
	ShaderType shaderType{ ShaderType::Draw };
	bool shouldRecompile{ true };
	bool compileOk{ false };
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	std::string geometryShaderSource;
	std::string tessControlShaderSource;
	std::string tessEvalShaderSource;
	std::string computeShaderSource;
	ProgramObject programObject;
	VertexArray vertexArrayObject;
	DrawStates drawStates;
	gl::GLbitfield computeBarrierBits{ 0 };
	int computeGroupSizeX{ 0 };
	int computeGroupSizeY{ 0 };
	int computeGroupSizeZ{ 0 };
	/*void loadFromShaderFile(const char *shaderId, sol::table table);
	void loadFromTable(sol::table config);
	void compile();*/
};

enum class PipelineStateMask
{
	VertexShader = (1 << 0),
	FragmentShader = (1 << 1),
	GeometryShader = (1 << 2),
	TessControlShader = (1 << 3),
	TessEvalShader = (1 << 4),
	Samplers = (1 << 5),
	Viewports = (1 << 6),
	BlendStates = (1 << 7),
	ComputeShader = (1 << 8),
	RasterizerState = (1 << 9),
	DepthStencilState = (1 << 10),
	InputLayout = (1 << 11),
	All = 0xFFFF
};
ENUM_BIT_FLAGS_OPERATORS(PipelineStateMask)

class AG_ENGINE_API Pipeline
{
public:
	// Create an empty pipeline of the specified type
	Pipeline(PipelineType type = PipelineType::Draw);
	// Load from source
	Pipeline(const char* templateSource, PipelineStateMask stateMask = PipelineStateMask::All);
	// Load a pipeline
	Pipeline(Cache& cache, const char* templatePath, PipelineStateMask stateMask = PipelineStateMask::All);
	// Destructor
	~Pipeline();

	// set individual shader sources
	void setVertexShader(std::string vs);
	void setFragmentShader(std::string fs);
	void setGeometryShader(std::string cs);
	void setTessControlShader(std::string tcs);
	void setTessEvalShader(std::string tes);
	// ignored if PipelineType != draw
	void setComputeShader(std::string cs);

	// Bind to a state group
	void operator()(StateGroup &stateGroup);

private: 
	std::shared_ptr<PipelineCacheObject> cached_;
};


} // namespace ag
