#include <autograph/engine/Shader.h>
#include <autograph/gl/Capture.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/support/Debug.h>
#include <iostream>

namespace ag {

//////////////////////////////////////////////
void PipelineState::loadFromTable(sol::table config) {
  // get shader type
  // if there is a "computeShader" source, then it's a compute shader
  // also isCompute = true, but that should be for Lua only
  sol::optional<std::string> computeShaderSourceOpt = config["computeShader"];
  if (computeShaderSourceOpt) {
    // This is a compute shader
    shaderType = ShaderType::Compute;
    computeShaderSource = *computeShaderSourceOpt;
    barrierBits = config.get_or("barrierBits", GL_ALL_BARRIER_BITS);
  } else {
    // Draw shader
    shaderType = ShaderType::Draw;
    {
      sol::table blendStates = config["blendState"];
      blendStates.for_each([this](sol::object key, sol::object value) {
        assert(key.get_type() == sol::type::number);
        assert(value.get_type() == sol::type::table);
        auto index = key.as<int>();
        auto table = value.as<sol::table>();
        auto &bs = drawStates.blendStates[index];
        bs.enabled = table.get_or("enabled", false);
        if (bs.enabled) {
          bs.modeRGB = table.get_or("modeRGB", GL_FUNC_ADD);
          bs.modeAlpha = table.get_or("modeAlpha", GL_FUNC_ADD);
          bs.funcSrcRGB = table.get_or("funcSrcRGB", GL_SRC_ALPHA);
          bs.funcDstRGB = table.get_or("funcDstRGB", GL_ONE_MINUS_SRC_ALPHA);
          bs.funcSrcAlpha = table.get_or("funcSrcAlpha", GL_ONE);
          bs.funcDstAlpha = table.get_or("funcDstAlpha", GL_ZERO);
        }
      });
    }

    {
      sol::table table = config["rasterizerState"];
      auto &rs = drawStates.rasterizerState;
      rs.fillMode = table.get_or("fillMode", GL_FILL);
      rs.cullMode = table.get_or("cullMode", GL_NONE);
      rs.frontFace = table.get_or("frontFace", GL_CCW);
      rs.depthBias = table.get_or("depthBias", 1.0f);
      rs.slopeScaledDepthBias = table.get_or("slopeScaledDepthBias", 1.0f);
      rs.depthClipEnable = table.get_or("depthClipEnable", false);
      rs.scissorEnable = table.get_or("scissorEnable", false);
    }

    {
      sol::table table = config["depthStencilState"];
      auto &dss = drawStates.depthStencilState;
      dss.depthTestEnable = table.get_or("depthTestEnable", false);
      dss.depthWriteEnable = table.get_or("depthWriteEnable", false);
      dss.stencilEnable = table.get_or("stencilEnable", false);
      if (dss.stencilEnable) {
        dss.stencilFace = table.get_or("stencilFace", GL_FRONT_AND_BACK);
        dss.stencilFunc = table.get_or("stencilFunc", 0);
        dss.stencilRef = table.get_or("stencilRef", 0);
        dss.stencilMask = table.get_or("stencilMask", 0xFFFFFFFF);
        dss.stencilOpSfail = table.get_or("stencilOpSfail", 0);
        dss.stencilOpDPFail = table.get_or("stencilOpDPFail", 0);
        dss.stencilOpDPPass = table.get_or("stencilOpDPPass", 0);
      }
    }

    {
      // viewports
      sol::optional<sol::table> viewports = config["viewports"];
      if (viewports) {
        viewports->for_each([this](sol::object key, sol::object value) {
          assert(key.get_type() == sol::type::number);
          assert(value.get_type() == sol::type::table);
          auto index = key.as<int>();
          auto table = value.as<sol::table>();
          auto &viewport = drawStates.viewports[index];
          viewport.x = table.get_or("x", 0.0f);
          viewport.y = table.get_or("y", 0.0f);
          viewport.w = table.get_or("w", 0.0f);
          viewport.h = table.get_or("h", 0.0f);
        });
      }
    }

    {
      // VAO
      sol::table layout = config["layout"];
      std::vector<gl::VertexAttribute> attribs;
      layout.for_each([&attribs, this](sol::object key, sol::object value) {
        assert(value.get_type() == sol::type::table);
        auto table = value.as<sol::table>();
        gl::VertexAttribute attrib;
        attrib.slot = table.get_or("buffer", 0);
        attrib.type = table.get_or("type", GL_FLOAT);
        attrib.size = table.get_or("size", 3);
        attrib.relativeOffset = table.get_or("relativeOffset", 0);
        attrib.normalized = table.get_or("normalized", false);
        attribs.push_back(attrib);
      });
      vertexArrayObject.initialize(attribs);
    }

    {
      // shaders
      vertexShaderSource = config["vertexShader"];
      fragmentShaderSource = config["fragmentShader"];
    }
  }

  shouldRecompile = true;
}

void PipelineState::compile() {
  if (shaderType == ShaderType::Draw) {
    programObject = gl::Program::create(vertexShaderSource.c_str(),
                                        fragmentShaderSource.c_str());
    if (!programObject.getLinkStatus()) {
      errorMessage("Failed to create program");
      debugMessage("====== Vertex shader: ======\n {}",
                   vertexShaderSource.c_str());
      debugMessage("====== Fragment shader: ======\n {}",
                   fragmentShaderSource.c_str());
      compileOk = false;
    } else {
      compileOk = true;
      drawStates.program = programObject.object();
      drawStates.vertexArray = vertexArrayObject.object();
    }
  } else {
    programObject = gl::Program::createCompute(computeShaderSource.c_str());
    if (!programObject.getLinkStatus()) {
      errorMessage("Failed to create program");
      debugMessage("====== Compute shader: ======\n {}",
                   computeShaderSource.c_str());
      compileOk = false;
    } else {
      compileOk = true;
      drawStates.program = programObject.object();
    }
  }
  shouldRecompile = false;
}

//////////////////////////////////////////////
std::shared_ptr<PipelineState>
PipelineStateCache::cachePipelineState(std::shared_ptr<PipelineState> ps) {
  // TODO
  states.push_back(ps);
  return ps;
}

void PipelineStateCache::releasePipelineState(
    std::shared_ptr<PipelineState> &ps) {
  // TODO No-op for now
}

int PipelineStateCache::getCachedPipelineStateCount() {
  return (int)states.size();
}

PipelineState *PipelineStateCache::getCachedPipelineState(int index) {
  return states[index].get();
}

void PipelineStateCache::reloadAll() {
	AG_DEBUG("Reloading all shaders");
	auto& L = detail::ensureShaderLuaStateInitialized();
  for (auto &p : states) {
    if (!p->origShaderID.empty())
      p->loadFromShaderFile(p->origShaderID.c_str(), L.create_table());
  }
}

PipelineStateCache &getPipelineStateCache() {
  static PipelineStateCache psc;
  return psc;
}

//////////////////////////////////////////////
void Shader::bindVertexArray(GLuint vao) {
  // TODO?
}

void Shader::setVertexShader(std::string vs) {
  // TODO make a copy if the cached pipeline is shared
  cached->vertexShaderSource = std::move(vs);
  cached->shouldRecompile = true;
}

void Shader::setFragmentShader(std::string fs) {
  // TODO make a copy if the cached pipeline is shared
  cached->fragmentShaderSource = std::move(fs);
  cached->shouldRecompile = true;
}

void Shader::setComputeShader(std::string cs) {
  // TODO make a copy if the cached pipeline is shared
  cached->computeShaderSource = std::move(cs);
  cached->shouldRecompile = true;
}

void Shader::addShaderDef(std::string kw, std::string def) {
  // TODO make a copy if the cached pipeline is shared
  // TODO
  cached->shouldRecompile = true;
}

void Shader::addShaderKeyword(std::string kw) {
  // TODO make a copy if the cached pipeline is shared
  // TODO
  cached->shouldRecompile = true;
}

void Shader::setBlendState(int index, const gl::BlendState &blendState) {
  // TODO make a copy if the cached pipeline is shared
  cached->drawStates.blendStates[index] = blendState;
}

void Shader::setViewport(int index, float x, float y, float w, float h) {
  // TODO make a copy if the cached pipeline is shared
  auto &vp = cached->drawStates.viewports[index];
  vp.x = x;
  vp.y = y;
  vp.w = w;
  vp.h = h;
}

void Shader::setRasterizerState(const gl::RasterizerState &rs) {
  // TODO make a copy if the cached pipeline is shared
  cached->drawStates.rasterizerState = rs;
}

void Shader::setDepthStencilState(const gl::DepthStencilState &ds) {
  // TODO make a copy if the cached pipeline is shared
  cached->drawStates.depthStencilState = ds;
}

//////////////////////////////////////////////
namespace detail {
ScriptContext &ensureShaderLuaStateInitialized() {
  static ScriptContext sc;
  static bool initialized = false;
  if (!initialized) {
    sc.script("require 'shader_utils'"); // Load shader utils
    initialized = true;
  }
  return sc;
}
}

//////////////////////////////////////////////
static void loadShaderFile(const char *shaderId) {
  auto &L = detail::ensureShaderLuaStateInitialized();
  AG_DEBUG("loadShaderFile: {}", shaderId);
  L.script(std::string{"require '"} + shaderId + "'");
}

/////////////////////////////////////////////////
void PipelineState::loadFromShaderFile(const char *shaderId, sol::table table) {
  auto &L = detail::ensureShaderLuaStateInitialized();
  // TODO load from cache and reuse instead of creating a new one each time
  origShaderID = shaderId;
  // Decompose shaderId into shader file + pass name
  std::string shaderIdStr{shaderId};
  auto p = shaderIdStr.find_last_of(':');
  if (p == std::string::npos) {
    errorMessage("Invalid shader identifier: {}", shaderId);
    return;
  }
  auto shaderFileId = shaderIdStr.substr(0, p);
  auto passId = shaderIdStr.substr(p + 1);
  // AG_DEBUG("createDrawPassInternal: {}", shaderId);
  try {
    loadShaderFile(shaderFileId.c_str());
    auto config = L["shader_utils"]["createShaderFromTemplate"](passId, table);
    loadFromTable(config);
  } catch (sol::error &e) {
    errorMessage("Error loading shader {}:\n\t{}", shaderId, e.what());
    throw;
  }
}

void Shader::initialize(const char *shaderId, sol::table table) {
  cached = std::make_shared<PipelineState>();
  cached->loadFromShaderFile(shaderId, table);
  cached = getPipelineStateCache().cachePipelineState(cached);
}

/////////////////////////////////////////////////
auto Shader::getDrawStates() -> const gl::DrawStates & {
  if (cached->shouldRecompile) {
    cached->compile();
  }
  return cached->drawStates;
}

void Shader::operator()(gl::StateGroup &stateGroup) {
  AG_FRAME_TRACE("shader id={}", cached->origShaderID);
  stateGroup.drawStates = getDrawStates();
}
}
