#include <autograph/Core/Support/Debug.h>
#include <autograph/Engine/ResourceManager.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/VertexArray.h>
#include <iostream>
#include <map>

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
    barrierBits =
        config.get_or("barrierBits", (gl::GLbitfield)gl::ALL_BARRIER_BITS);
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
          bs.modeRGB = table.get_or("modeRGB", (gl::GLenum)gl::FUNC_ADD);
          bs.modeAlpha = table.get_or("modeAlpha", (gl::GLenum)gl::FUNC_ADD);
          bs.funcSrcRGB = table.get_or("funcSrcRGB", (gl::GLenum)gl::SRC_ALPHA);
          bs.funcDstRGB =
              table.get_or("funcDstRGB", (gl::GLenum)gl::ONE_MINUS_SRC_ALPHA);
          bs.funcSrcAlpha = table.get_or("funcSrcAlpha", (gl::GLenum)gl::ONE);
          bs.funcDstAlpha = table.get_or("funcDstAlpha", (gl::GLenum)gl::ZERO);
        }
      });
    }

    {
      sol::table table = config["rasterizerState"];
      auto &rs = drawStates.rasterizerState;
      rs.fillMode = table.get_or("fillMode", (gl::GLenum)gl::FILL);
      rs.cullMode = table.get_or("cullMode", (gl::GLenum)gl::NONE);
      rs.frontFace = table.get_or("frontFace", (gl::GLenum)gl::CCW);
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
        dss.stencilFace =
            table.get_or("stencilFace", (gl::GLenum)gl::FRONT_AND_BACK);
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
      std::vector<VertexAttribute> attribs;
      layout.for_each([&attribs, this](sol::object key, sol::object value) {
        assert(value.get_type() == sol::type::table);
        auto table = value.as<sol::table>();
        VertexAttribute attrib;
        attrib.slot = table.get_or("buffer", 0);
        attrib.type = table.get_or("type", (gl::GLenum)gl::FLOAT);
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
    programObject = ProgramObject::create(vertexShaderSource.c_str(),
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
    programObject = ProgramObject::createCompute(computeShaderSource.c_str());
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
  auto &L = detail::ensureShaderLuaStateInitialized();
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
void Shader::bindVertexArray(gl::GLuint vao) {
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

void Shader::setBlendState(int index, const BlendState &blendState) {
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

void Shader::setRasterizerState(const RasterizerState &rs) {
  // TODO make a copy if the cached pipeline is shared
  cached->drawStates.rasterizerState = rs;
}

void Shader::setDepthStencilState(const DepthStencilState &ds) {
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
} // namespace detail

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
auto Shader::getDrawStates() -> const DrawStates & {
  if (cached->shouldRecompile) {
    cached->compile();
  }
  return cached->drawStates;
}

void Shader::operator()(StateGroup &stateGroup) {
  AG_FRAME_TRACE("shader id={}", cached->origShaderID);
  stateGroup.drawStates = getDrawStates();
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

struct ShaderDefine {
  std::string define;
  std::string value;
};

struct ShaderSource {
  std::string source;
};

// A pipeline template, possibly loaded from a file
struct CachedPipelineTemplateFile : CacheObject {
  CachedPipelineTemplateFile(const char *path) : CacheObject(path) {}

  PipelineStateMask loadMask;
};

// A pipeline, possibly referencing a template file
struct CachedPipeline : CacheObject {
  CachedPipeline(const char *id) : CacheObject{id} {}

  //
  // Initialize this pipeline from a Lua table
  void initializeFromTable(CachedPipelineTemplateFile &tplFile,
                           sol::table config, PipelineStateMask loadMask) {
    //-----------------------------------------
    using sol::optional;
    using sol::object;
    using sol::type;
    using sol::table;

    //-----------------------------------------
    auto warnStateGroupUndefined = [&](const char *sgName) {
      warningMessage("[{}] State group '{}' present in loading mask is not "
                     "defined in the file.",
                     tplFile.getPath(), sgName);
    };

    auto warnStateGroupNotInMask = [&](const char *sgName) {
      warningMessage("[{}] File defines state group '{}' but is not present in "
                     "the loading mask.",
                     tplFile.getPath(), sgName);
    };

    //-----------------------------------------
    optional<table> tblBlend = config["blendState"];
    optional<table> tblRS = config["rasterizerState"];
    optional<table> tblDSS = config["depthStencilState"];
    optional<table> tblViewports = config["viewports"];
    optional<table> tblLayout = config["layout"];

    //-----------------------------------------
    // Load blend states
    if (!!(loadMask & PipelineStateMask::BlendStates)) {
      if (tblBlend) {
        auto tbl = *tblBlend;
        tbl.for_each([this](object key, object value) {
          assert(key.get_type() == type::number);
          assert(value.get_type() == type::table);
          auto index = key.as<int>();
          auto tbl = value.as<table>();
          if (index >= drawStates.blendStates.size()) {
            errorMessage("index out of range ({})", index);
            return;
          }
          auto &bs = drawStates.blendStates[index];
          bs.enabled = tbl.get_or("enabled", bs.enabled);
          if (bs.enabled) {
            bs.modeRGB = tbl.get_or("modeRGB", bs.modeRGB);
            bs.modeAlpha = tbl.get_or("modeAlpha", bs.modeAlpha);
            bs.funcSrcRGB = tbl.get_or("funcSrcRGB", bs.funcSrcRGB);
            bs.funcDstRGB = tbl.get_or("funcDstRGB", bs.funcDstRGB);
            bs.funcSrcAlpha = tbl.get_or("funcSrcAlpha", bs.funcSrcAlpha);
            bs.funcDstAlpha = tbl.get_or("funcDstAlpha", bs.funcDstAlpha);
          }
        });
      } else {
        warnStateGroupUndefined("Blend");
      }
    } else if (tblBlend) {
      warnStateGroupNotInMask("Blend");
    }

    //-----------------------------------------
    // Load rasterizer states
    if (!!(loadMask & PipelineStateMask::RasterizerState)) {
      if (tblRS) {
        table tbl = *tblRS;
        auto &rs = drawStates.rasterizerState;
        rs.fillMode = tbl.get_or("fillMode", rs.fillMode);
        rs.cullMode = tbl.get_or("cullMode", rs.cullMode);
        rs.frontFace = tbl.get_or("frontFace", rs.frontFace);
        rs.depthBias = tbl.get_or("depthBias", rs.depthBias);
        rs.slopeScaledDepthBias =
            tbl.get_or("slopeScaledDepthBias", rs.slopeScaledDepthBias);
        rs.depthClipEnable = tbl.get_or("depthClipEnable", rs.depthClipEnable);
        rs.scissorEnable = tbl.get_or("scissorEnable", rs.scissorEnable);
      } else {
        warnStateGroupUndefined("Rasterizer");
      }
    } else if (tblRS) {
      warnStateGroupNotInMask("Rasterizer");
    }

    //-----------------------------------------
    // Load depth-stencil states
    if (!!(loadMask & PipelineStateMask::DepthStencilState)) {
      if (tblDSS) {
        table tbl = *tblDSS;
        auto &dss = drawStates.depthStencilState;
        dss.depthTestEnable = tbl.get_or("depthTestEnable", false);
        dss.depthWriteEnable = tbl.get_or("depthWriteEnable", false);
        dss.stencilEnable = tbl.get_or("stencilEnable", false);
        if (dss.stencilEnable) {
          dss.stencilFace =
              tbl.get_or("stencilFace", (gl::GLenum)gl::FRONT_AND_BACK);
          dss.stencilFunc = tbl.get_or("stencilFunc", 0);
          dss.stencilRef = tbl.get_or("stencilRef", 0);
          dss.stencilMask = tbl.get_or("stencilMask", 0xFFFFFFFF);
          dss.stencilOpSfail = tbl.get_or("stencilOpSfail", 0);
          dss.stencilOpDPFail = tbl.get_or("stencilOpDPFail", 0);
          dss.stencilOpDPPass = tbl.get_or("stencilOpDPPass", 0);
        }
      } else {
        warnStateGroupUndefined("Depth-stencil");
      }
    } else if (tblDSS) {
      warnStateGroupNotInMask("Depth-stencil");
    }

    //-----------------------------------------
    // Load viewports
    if (!!(loadMask & PipelineStateMask::Viewports)) {
      if (tblViewports) {
        auto tbl = *tblViewports;
        tbl.for_each([this](object key, object value) {
          assert(key.get_type() == type::number);
          assert(value.get_type() == type::table);
          auto index = key.as<int>();
          auto tblV = value.as<table>();
          if (index >= drawStates.viewports.size()) {
            errorMessage("index out of range ({})", index);
            return;
          }
          auto &viewport = drawStates.viewports[index];
          viewport.x = tblV.get_or("x", viewport.x);
          viewport.y = tblV.get_or("y", viewport.y);
          viewport.w = tblV.get_or("w", viewport.w);
          viewport.h = tblV.get_or("h", viewport.h);
        });
      } else {
        warnStateGroupUndefined("Viewports");
      }
    } else if (tblViewports) {
      warnStateGroupNotInMask("Viewports");
    }

    //-----------------------------------------
    // Load input layout
    if (!!(loadMask & PipelineStateMask::InputLayout)) {
      if (tblLayout) {
        auto tbl = *tblLayout;
        std::vector<VertexAttribute> attribs;
        tbl.for_each([&attribs, this](object key, object value) {
          assert(value.get_type() == type::table);
          auto tblAttrib = value.as<table>();
          VertexAttribute attrib;
          attrib.slot = tblAttrib.get_or("buffer", 0);
          attrib.type = tblAttrib.get_or("type", (gl::GLenum)gl::FLOAT);
          attrib.size = tblAttrib.get_or("size", 3);
          attrib.relativeOffset = tblAttrib.get_or("relativeOffset", 0);
          attrib.normalized = tblAttrib.get_or("normalized", false);
          attribs.push_back(attrib);
        });
        vertexArrayObject.initialize(attribs);
      } else {
        warnStateGroupUndefined("Layout");
      }
    } else if (tblLayout) {
      warnStateGroupNotInMask("Layout");
    }

    //-----------------------------------------
    // Add shader defines
    if (table tblDefines = config["shaderDefines"]) {
      if (!!(loadMask & PipelineStateMask::ShaderDefines)) {
        tblDefines.for_each([this](object key, object value) {
          assert(value.get_type() == type::string);
          assert(key.get_type() == type::string);
          shaderDefines.push_back(
              ShaderDefine{key.as<std::string>(), value.as<std::string>()});
        });
      } else {
        warningMessage("[{}] File contains shader defines, but the "
                       "ShaderDefines bit is not set in the load mask: the "
                       "defines will be ignored",
                       origPath);
      }
    }

    //-----------------------------------------
    // Load shader sources
    if (optional<std::string> shaderSrc = config["shaderSource"]) {
      tplFile.shaderSources.push_back(ShaderSource{*shaderSrc});
    }
  }

  void loadFromTemplate(const char *source) {
    // 1. get lua state
    auto &lua = detail::ensureShaderLuaStateInitialized();
    // 2. shader function callback
    lua["Shader"] = [&](sol::table table) {

    };
    // 2. execute template script
    lua.script(source);
    // 3. copy defined values
  }

  // Hash of the pipeline state
  uint64_t hash = 0;
  // Shader type (Draw or Compute)
  ShaderType shaderType = ShaderType::Draw;
  // Shader sources (combined)
  std::vector<ShaderSource> combinedShaderSources;
  // Shader definitions
  std::vector<ShaderDefine> shaderDefines;
  // Memory barriers to apply at the end of a compute shader
  gl::GLbitfield computeBarrierBits = 0;
  // List of pipeline template files that this state relies on
  std::vector<std::shared_ptr<CachedPipelineTemplateFile>> templateFiles;
  // List of shader sources (that may come from different template files)
  std::vector<ShaderSource> shaderSources;
  std::vector<ShaderDefine> shaderDefines;
  // Compute group size
  int computeGroupSizeX = 0;
  int computeGroupSizeY = 0;
  int computeGroupSizeZ = 0;
  // Draw states
  DrawStates drawStates;
  // Compiled program object (empty if compileOk == false)
  ProgramObject programObject;
  // Vertex array object
  VertexArray vertexArrayObject;
  // Should this pipeline be recompiled at the next usage
  bool shouldRecompile = true;
  // Is program object currently valid
  bool compileOk = false;
}; // namespace ag

Pipeline::Pipeline(PipelineType type = PipelineType::Draw) {}

// Load from source
Pipeline::Pipeline(const char *source) {}

// Load a pipeline from a cache
Pipeline::Pipeline(Cache &cache, const char *path) {
  // Try to find an entry in the cache
  cached_ = cache.getObjectOfType<CachedPipeline>(path);
  // Not found: load it and cache it
  if (!cached_) {
    cached_ = loadPipelineFromPath(path);
    cache.addObject(cached_);
  }
}
} // namespace ag
