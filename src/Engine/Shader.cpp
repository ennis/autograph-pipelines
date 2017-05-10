#include "ShaderPreprocessor.h"
#include <autograph/Core/Support/Debug.h>
#include <autograph/Engine/ResourceManager.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/VertexArray.h>
#include <fstream>
#include <iostream>
#include <map>

namespace ag {

//////////////////////////////////////////////
/*void PipelineState::loadFromTable(sol::table config) {
  // get shader type
  // if there is a "computeShader" source, then it's a compute shader
  // also isCompute = true, but that should be for Lua only
  sol::optional<std::string> computeShaderSourceOpt = config["computeShader"];
  if (computeShaderSourceOpt) {
    // This is a compute shader
    shaderType = GPUPipelineType::Compute;
    computeShaderSource = *computeShaderSourceOpt;
    barrierBits =
        config.get_or("barrierBits", (gl::GLbitfield)gl::ALL_BARRIER_BITS);
  } else {
    // Draw shader
    shaderType = GPUPipelineType::Draw;
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
}*/

/*void PipelineState::compile() {
  if (shaderType == GPUPipelineType::Draw) {
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
}*/

//////////////////////////////////////////////
/*std::shared_ptr<PipelineState>
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
}*/

//////////////////////////////////////////////
/*void Shader::bindVertexArray(gl::GLuint vao) {
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
}*/

//////////////////////////////////////////////
namespace detail {
ScriptContext &ensureShaderLuaStateInitialized() {
  static ScriptContext sc;
  static bool initialized = false;
  if (!initialized) {
    sc.script("require 'ShaderUtils'"); // Load shader utils
    initialized = true;
  }
  return sc;
}
} // namespace detail

/////////////////////////////////////////////////
/*void PipelineState::loadFromShaderFile(const char *shaderId, sol::table table)
{ auto &L = detail::ensureShaderLuaStateInitialized();
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
}*/

/*void Shader::initialize(const char *shaderId, sol::table table) {
  cached = std::make_shared<PipelineState>();
  cached->loadFromShaderFile(shaderId, table);
  cached = getPipelineStateCache().cachePipelineState(cached);
}*/

/////////////////////////////////////////////////
/*auto Shader::getDrawStates() -> const DrawStates & {
  if (cached->shouldRecompile) {
    cached->compile();
  }
  return cached->drawStates;
}

void Shader::operator()(StateGroup &stateGroup) {
  AG_FRAME_TRACE("shader id={}", cached->origShaderID);
  stateGroup.drawStates = getDrawStates();
}*/

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

// Helper struct for loadPipelineDescFromTable
struct GPUPipelineFile {
  GPUPipelineType pipelineType;
  DepthStencilState depthStencilState;
  RasterizerState rasterizerState;
  std::array<BlendState, 8> blendStates;
  std::vector<VertexAttribute> vertexAttributes;
  std::vector<std::string> shaderDefines;
  ShaderSources shaderSources;
  gl::GLbitfield computeBarrierBits{0};
  int computeGroupSizeX{0};
  int computeGroupSizeY{0};
  int computeGroupSizeZ{0};
};

// A pipeline template, possibly loaded from a file
struct CachedGPUPipelineFile : CacheObject {
  CachedGPUPipelineFile(const char *path) : CacheObject(path) {}
  GPUPipelineFile desc;
};

static std::string loadText(const char *path) {
  std::ifstream fileIn(path, std::ios::in);
  if (!fileIn.is_open()) {
    std::cerr << "Could not open file " << path << std::endl;
    throw std::runtime_error("Could not open file");
  }
  std::string str;
  str.assign((std::istreambuf_iterator<char>(fileIn)),
             std::istreambuf_iterator<char>());
  return str;
}

static void loadGPUPipelineFromTable(const char *origPath, sol::table config,
                                     GPUPipelineFile &out) {
  //-----------------------------------------
  using sol::optional;
  using sol::object;
  using sol::type;
  using sol::table;

  //-----------------------------------------
  /*auto warnStateGroupUndefined = [&](const char *sgName) {
    warningMessage("[{}] State group '{}' present in loading mask is not "
                   "defined in the file.",
                   origPath, sgName);
  };

  auto warnStateGroupNotInMask = [&](const char *sgName) {
    warningMessage("[{}] File defines state group '{}' but is not present in "
                   "the loading mask.",
                   origPath, sgName);
  };*/

  //-----------------------------------------
  optional<table> tblBlend = config["blendState"];
  optional<table> tblRS = config["rasterizerState"];
  optional<table> tblDSS = config["depthStencilState"];
  optional<table> tblViewports = config["viewports"];
  optional<table> tblLayout = config["layout"];

  //-----------------------------------------
  // Load blend states
  // if (!!(loadMask & PipelineStateMask::BlendStates)) {
  if (tblBlend) {
    auto tbl = *tblBlend;
    tbl.for_each([&](object key, object value) {
      assert(key.get_type() == type::number);
      assert(value.get_type() == type::table);
      auto index = key.as<int>();
      auto tbl = value.as<table>();
      if (index >= out.blendStates.size()) {
        errorMessage("index out of range ({})", index);
        return;
      }
      auto &bs = out.blendStates[index];
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
  }
  // else {
  //    warnStateGroupUndefined("Blend");
  //  }
  //} else if (tblBlend) {
  //  warnStateGroupNotInMask("Blend");
  //}

  //-----------------------------------------
  // Load rasterizer states
  // if (!!(loadMask & PipelineStateMask::RasterizerState)) {
  if (tblRS) {
    table tbl = *tblRS;
    auto &rs = out.rasterizerState;
    rs.fillMode = tbl.get_or("fillMode", rs.fillMode);
    rs.cullMode = tbl.get_or("cullMode", rs.cullMode);
    rs.frontFace = tbl.get_or("frontFace", rs.frontFace);
    rs.depthBias = tbl.get_or("depthBias", rs.depthBias);
    rs.slopeScaledDepthBias =
        tbl.get_or("slopeScaledDepthBias", rs.slopeScaledDepthBias);
    rs.depthClipEnable = tbl.get_or("depthClipEnable", rs.depthClipEnable);
    rs.scissorEnable = tbl.get_or("scissorEnable", rs.scissorEnable);
  }
  // else {
  //   warnStateGroupUndefined("Rasterizer");
  // }
  //} else if (tblRS) {
  //  warnStateGroupNotInMask("Rasterizer");
  //}

  //-----------------------------------------
  // Load depth-stencil states
  // if (!!(loadMask & PipelineStateMask::DepthStencilState)) {
  if (tblDSS) {
    table tbl = *tblDSS;
    auto &dss = out.depthStencilState;
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
  }
  // else {
  //    warnStateGroupUndefined("Depth-stencil");
  //  }
  // } else if (tblDSS) {
  //  warnStateGroupNotInMask("Depth-stencil");
  //}

  //-----------------------------------------
  // Load viewports
  // if (!!(loadMask & PipelineStateMask::Viewports)) {
  /*if (tblViewports) {
    auto tbl = *tblViewports;
    tbl.for_each([&](object key, object value) {
      assert(key.get_type() == type::number);
      assert(value.get_type() == type::table);
      auto index = key.as<int>();
      auto tblV = value.as<table>();
      if (index >= out.viewports.size()) {
        errorMessage("index out of range ({})", index);
        return;
      }
      auto &viewport = out.viewports[index];
      viewport.x = tblV.get_or("x", viewport.x);
      viewport.y = tblV.get_or("y", viewport.y);
      viewport.w = tblV.get_or("w", viewport.w);
      viewport.h = tblV.get_or("h", viewport.h);
    });
  }*/
  // else {
  //    warnStateGroupUndefined("Viewports");
  //  }
  //} else if (tblViewports) {
  //  warnStateGroupNotInMask("Viewports");
  //}

  //-----------------------------------------
  // Load input layout
  // if (!!(loadMask & PipelineStateMask::InputLayout)) {
  if (tblLayout) {
    auto tbl = *tblLayout;
    out.vertexAttributes.clear();
    tbl.for_each([&](object key, object value) {
      assert(value.get_type() == type::table);
      auto tblAttrib = value.as<table>();
      VertexAttribute attrib;
      attrib.slot = tblAttrib.get_or("buffer", 0);
      attrib.type = tblAttrib.get_or("type", (gl::GLenum)gl::FLOAT);
      attrib.size = tblAttrib.get_or("size", 3);
      attrib.relativeOffset = tblAttrib.get_or("relativeOffset", 0);
      attrib.normalized = tblAttrib.get_or("normalized", false);
      out.vertexAttributes.push_back(attrib);
    });
  }
  // else {
  //    warnStateGroupUndefined("Layout");
  //  }
  //} else if (tblLayout) {
  //  warnStateGroupNotInMask("Layout");
  //}

  //-----------------------------------------
  // Add shader defines
  if (table tblDefines = config["shaderDefines"]) {
    // if (!!(loadMask & PipelineStateMask::ShaderDefines)) {
    tblDefines.for_each([&](object key, object value) {
      assert(value.get_type() == type::string);
      assert(key.get_type() == type::string);
      out.shaderDefines.push_back(value.as<std::string>());
    });
    //} else {
    //  warningMessage("[{}] File contains shader defines, but the "
    //                 "ShaderDefines bit is not set in the load mask: the "
    //                 "defines will be ignored",
    //                 origPath);
    //}
  }

  //-----------------------------------------
  // Load shader sources
  if (optional<std::string> shaderSrc = config["shaderSource"]) {
    // assume a 'combined shader source': put a copy for all stages
    // the sources must be replaced (and the includes resolved) with their
    // preprocessed version before handing the PipelineDesc to the compiler The
    // preprocessor will resolve includes and extract source code relevant to
    // each stage with the _VERTEX_, _FRAGMENT_, _GEOMETRY_, _TESS_EVAL_ and
    // _TESS_CONTROL_ shaders
    out.shaderSources.vertexShader.source = *shaderSrc;
    out.shaderSources.vertexShader.path = origPath;
    out.shaderSources.fragmentShader.source = *shaderSrc;
    out.shaderSources.fragmentShader.path = origPath;
    out.shaderSources.tessControlShader.source = *shaderSrc;
    out.shaderSources.tessControlShader.path = origPath;
    out.shaderSources.tessEvalShader.source = *shaderSrc;
    out.shaderSources.tessEvalShader.path = origPath;
    out.shaderSources.geometryShader.source = *shaderSrc;
    out.shaderSources.geometryShader.path = origPath;
    out.shaderSources.computeShader.source = *shaderSrc;
    out.shaderSources.computeShader.path = origPath;
  } else if (optional<std::string> shaderSrc = config["shaderFile"]) {
    // load from given path (relative to .lua)
    auto parentPath = ResourceManager::getParentPath(origPath);
    auto srcpath = parentPath + "/" + *shaderSrc;
    auto src = loadText(srcpath.c_str());
    // as before, assume a combined shader source
    out.shaderSources.vertexShader.source = src;
    out.shaderSources.vertexShader.path = srcpath;
    out.shaderSources.fragmentShader.source = src;
    out.shaderSources.fragmentShader.path = srcpath;
    out.shaderSources.tessControlShader.source = src;
    out.shaderSources.tessControlShader.path = srcpath;
    out.shaderSources.tessEvalShader.source = src;
    out.shaderSources.tessEvalShader.path = srcpath;
    out.shaderSources.geometryShader.source = src;
    out.shaderSources.geometryShader.path = srcpath;
    out.shaderSources.computeShader.source = src;
    out.shaderSources.computeShader.path = srcpath;
  }
}

static void loadGPUPipelineFile(const char *path, const char *tableName,
                                GPUPipelineFile &out) {
  // get Lua context
  auto &context = detail::ensureShaderLuaStateInitialized();
  // load script into Lua context
  try {
    context.script_file(path);
    loadGPUPipelineFromTable(path, context[tableName], out);
  } catch (sol::error &err) {
    errorMessage("Error loading pipeline file {}:", path);
    errorMessage(" -> {}", err.what());
  }
}

// 'Compiled' pipeline state
// Should be in Gfx
struct GraphicsPipelineState : CacheObject {
  GraphicsPipelineState(const char *hash) : CacheObject{hash} {}

  // Bind to state group
  void operator()(StateGroup &sg) {
    sg.vertexArray = vao.object();
    sg.program = prog.object();
    sg.depthStencilState = depthStencilState;
    sg.rasterizerState = rasterizerState;
    sg.blendStates = blendStates;
  }

  DepthStencilState depthStencilState;
  RasterizerState rasterizerState;
  std::array<BlendState, 8> blendStates;
  VertexArray vao;
  ProgramObject prog;
};

// Complete description of a compute pipeline
// Should be in Gfx
struct ComputePipelineState : CacheObject {
  ComputePipelineState(const char *hash) : CacheObject{hash} {}

  void operator()(StateGroup &sg) { sg.program = prog.object(); }

  ProgramObject prog;
  gl::GLbitfield memoryBarrierBits = 0;
  int computeGroupSizeX = 0;
  int computeGroupSizeY = 0;
  int computeGroupSizeZ = 0;
};

// Complete description of a graphics pipeline
struct GraphicsPipelineDesc {
  // Depth-stencil
  DepthStencilState depthStencilState;
  // Rasterizer
  RasterizerState rasterizerState;
  // Blend (OM)
  std::array<BlendState, 8> blendStates;
  // Vertex assembly (IA)
  span<VertexAttribute> inputLayout;
  const char *vertexShader = nullptr;
  const char *fragmentShader = nullptr;
  const char *geometryShader = nullptr;
  const char *tessControlShader = nullptr;
  const char *tessEvalShader = nullptr;
  const char *vertexShaderPath = "<unknown>";
  const char *fragmentShaderPath = "<unknown>";
  const char *geometryShaderPath = "<unknown>";
  const char *tessControlShaderPath = "<unknown>";
  const char *tessEvalShaderPath = "<unknown>";
};

// Complete description of a compute pipeline
struct ComputePipelineDesc {
  const char *computeShader = nullptr;
  const char *computeShaderPath = "<unknown>";
  gl::GLbitfield memoryBarrierBits = 0;
};

GPUPipeline::GPUPipeline(GPUPipelineType type, Cache *cache)
    : type_{type}, cache_{cache} {}

void GPUPipeline::operator()(StateGroup &sg) {
  if (!compiled_) {
    compile();
  }
  ag::visit([&](auto &&arg) { (*arg)(sg); }, state_);
}

// Load from a pipeline file
GPUPipeline::GPUPipeline(GPUPipelineType type, const char *pathSubpath,
                         LoadMask stateMask, Cache *cache)
    : type_{type}, cache_{cache}, loadMask_{stateMask} {
  auto path = ResourceManager::getFilesystemPath(pathSubpath);
  shaderName_ = ResourceManager::getSubpathPart(pathSubpath);
  if (shaderName_.empty()) {
    shaderName_ = "_G";
  }

  // create a cache entry for the file
  if (cache)
    templateFile_ = cache->getObjectOfType<CachedGPUPipelineFile>(path.c_str());
  if (!templateFile_) {
    templateFile_ = std::make_shared<CachedGPUPipelineFile>(path.c_str());
    // load pipeline file
    loadGPUPipelineFile(path.c_str(), shaderName_.c_str(), templateFile_->desc);
  }

  // now import our state
  if (!!(loadMask_ & LoadMask::RasterizerState))
    rasterizerState = templateFile_->desc.rasterizerState;
  if (!!(loadMask_ & LoadMask::BlendStates))
    blendStates = templateFile_->desc.blendStates;
  if (!!(loadMask_ & LoadMask::DepthStencilState))
    depthStencilState = templateFile_->desc.depthStencilState;
  // if (!!(loadMask_ & LoadMask::Viewports))
  //  viewports = templateFile->desc.viewports;
  if (!!(loadMask_ & LoadMask::InputLayout))
    vertexAttributes = templateFile_->desc.vertexAttributes;
  // if (!!(loadMask_ & PipelineStateMask::ShaderDefines))
  shaderDefines = templateFile_->desc.shaderDefines;
  if (!!(loadMask_ & LoadMask::VertexShader)) {
    vertexShader = templateFile_->desc.shaderSources.vertexShader.source;
    vertexShaderPath = templateFile_->desc.shaderSources.vertexShader.path;
  }
  if (!!(loadMask_ & LoadMask::FragmentShader)) {
    fragmentShader = templateFile_->desc.shaderSources.fragmentShader.source;
    fragmentShaderPath = templateFile_->desc.shaderSources.fragmentShader.path;
  }
  if (!!(loadMask_ & LoadMask::GeometryShader)) {
    geometryShader = templateFile_->desc.shaderSources.geometryShader.source;
    geometryShaderPath = templateFile_->desc.shaderSources.geometryShader.path;
  }
  if (!!(loadMask_ & LoadMask::TessControlShader)) {
    tessControlShader =
        templateFile_->desc.shaderSources.tessControlShader.source;
    tessControlShaderPath =
        templateFile_->desc.shaderSources.tessControlShader.path;
  }
  if (!!(loadMask_ & LoadMask::TessEvalShader)) {
    tessEvalShader = templateFile_->desc.shaderSources.tessEvalShader.source;
    tessEvalShaderPath = templateFile_->desc.shaderSources.tessEvalShader.path;
  }
  if (!!(loadMask_ & LoadMask::ComputeShader)) {
    computeShader = templateFile_->desc.shaderSources.computeShader.source;
    computeShaderPath = templateFile_->desc.shaderSources.computeShader.path;
    memoryBarrierBits = templateFile_->desc.computeBarrierBits;
    computeGroupSizeX = templateFile_->desc.computeGroupSizeX;
    computeGroupSizeY = templateFile_->desc.computeGroupSizeY;
    computeGroupSizeZ = templateFile_->desc.computeGroupSizeZ;
  }

  if (cache)
    cache->addObject(templateFile_);
}

static std::shared_ptr<GraphicsPipelineState>
createGraphicsPipelineState(const char *name, const GraphicsPipelineDesc &desc,
                            Cache *cache = nullptr) {
  auto state = std::make_shared<GraphicsPipelineState>(name);
  state->rasterizerState = desc.rasterizerState;
  state->blendStates = desc.blendStates;
  state->depthStencilState = desc.depthStencilState;
  // init VAO
  state->vao.initialize(desc.inputLayout);
  // create program
  state->prog = ProgramObject::create(
      desc.vertexShader, desc.fragmentShader, desc.geometryShader,
      desc.tessControlShader, desc.tessEvalShader);
  if (!state->prog.getLinkStatus()) {
    errorMessage("Failed to create program");
    debugMessage("====== Vertex shader ({}): ======\n {}",
                 desc.vertexShaderPath, desc.vertexShader);
    debugMessage("====== Fragment shader ({}): ======\n {}",
                 desc.fragmentShaderPath, desc.fragmentShader);
    return nullptr;
  } else {
    return state;
  }
}

static std::shared_ptr<ComputePipelineState>
createComputePipelineState(const char *name, const ComputePipelineDesc &desc,
                           Cache *cache = nullptr) {
  auto state = std::make_shared<ComputePipelineState>(name);
  state->prog = ProgramObject::createCompute(desc.computeShader);
  if (!state->prog.getLinkStatus()) {
    errorMessage("Failed to create program");
    debugMessage("====== Compute shader ({}): ======\n {}",
                 desc.computeShaderPath, desc.computeShader);
    return nullptr;
  } else {
    return state;
  }
}

void GPUPipeline::compile() {
  AG_DEBUG("Compiling: {}${}", templateFile_->getPath(), shaderName_.c_str());
  std::vector<const char *> macros;
  for (const auto &d : shaderDefines)
    macros.push_back(d.c_str());
  for (const auto &d : additionalShaderDefines)
    macros.push_back(d.c_str());

  if (type_ == GPUPipelineType::Graphics) {
    // preprocess sources
    ShaderSources shaderSources;
    shaderSources.vertexShader.source = vertexShader.c_str();
    shaderSources.vertexShader.path = vertexShaderPath.c_str();
    shaderSources.fragmentShader.source = fragmentShader.c_str();
    shaderSources.fragmentShader.path = fragmentShaderPath.c_str();
    shaderSources.geometryShader.source = geometryShader.c_str();
    shaderSources.geometryShader.path = geometryShaderPath.c_str();
    shaderSources.tessControlShader.source = tessControlShader.c_str();
    shaderSources.tessControlShader.path = tessControlShaderPath.c_str();
    shaderSources.tessEvalShader.source = tessEvalShader.c_str();
    shaderSources.tessEvalShader.path = tessEvalShaderPath.c_str();
    preprocessMultiShaderSources(shaderSources, macros, {});
    // fill pipeline desc struct
    GraphicsPipelineDesc desc;
    desc.inputLayout = vertexAttributes;
    desc.blendStates = blendStates;
    desc.depthStencilState = depthStencilState;
    desc.rasterizerState = rasterizerState;
    desc.vertexShader = shaderSources.vertexShader.source.c_str();
    desc.fragmentShader = shaderSources.fragmentShader.source.c_str();
    desc.geometryShader = shaderSources.geometryShader.source.empty()
                              ? nullptr
                              : shaderSources.geometryShader.source.c_str();
    desc.tessControlShader =
        shaderSources.tessControlShader.source.empty()
            ? nullptr
            : shaderSources.tessControlShader.source.c_str();
    desc.tessEvalShader = shaderSources.tessEvalShader.source.empty()
                              ? nullptr
                              : shaderSources.tessEvalShader.source.c_str();

    desc.vertexShaderPath = shaderSources.vertexShader.path.c_str();
    desc.fragmentShaderPath = shaderSources.fragmentShader.path.c_str();
    desc.geometryShaderPath = shaderSources.geometryShader.path.c_str();
    desc.tessControlShaderPath = shaderSources.tessControlShader.path.c_str();
    desc.tessEvalShaderPath = shaderSources.tessEvalShader.path.c_str();
    // create pipeline
    state_ =
        createGraphicsPipelineState(templateFile_->getPath(), desc, cache_);
  } else if (type_ == GPUPipelineType::Compute) {
    // preprocess source
    std::string ppComputeShaderSource =
        preprocessShaderSource(computeShader, computeShaderPath.c_str(),
                               ShaderStage::Compute, macros, {});
    // create pipeline
    ComputePipelineDesc desc;
    desc.computeShader = ppComputeShaderSource.c_str();
    desc.computeShaderPath = computeShaderPath.c_str();
    state_ = createComputePipelineState(templateFile_->getPath(), desc, cache_);
  }

  // if (state_) {
  compiled_ = true;
  //}
}

// Destructor
GPUPipeline::~GPUPipeline() {}

} // namespace ag
