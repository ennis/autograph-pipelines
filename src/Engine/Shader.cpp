#include "ShaderPreprocessor.h"
#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/md5.h>
#include <autograph/Engine/ResourceManager.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/VertexArray.h>
#include <fstream>
#include <iostream>
#include <map>

namespace ag {

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

struct CachedProgramObject : CacheObject {
  CachedProgramObject(const char *hashstr, ProgramObject &&prog_)
      : CacheObject{hashstr}, prog{std::move(prog_)} {}

  ProgramObject prog;
};

// 'Compiled' pipeline state
// Should be in Gfx
struct GraphicsPipelineState : CacheObject {
  GraphicsPipelineState(const char *hash) : CacheObject{hash} {}

  // Bind to state group
  void operator()(StateGroup &sg) {
    sg.vertexArray = vao.object();
    sg.program = cachedProgram->prog.object();
    sg.depthStencilState = depthStencilState;
    sg.rasterizerState = rasterizerState;
    sg.blendStates = blendStates;
  }

  DepthStencilState depthStencilState;
  RasterizerState rasterizerState;
  std::array<BlendState, 8> blendStates;
  VertexArray vao;
  std::shared_ptr<CachedProgramObject> cachedProgram;
};

// Complete description of a compute pipeline
// Should be in Gfx
struct ComputePipelineState : CacheObject {
  ComputePipelineState(const char *hash) : CacheObject{hash} {}

  void operator()(StateGroup &sg) { sg.program = cachedProgram->prog.object(); }

  std::shared_ptr<CachedProgramObject> cachedProgram;
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
                         LoadMask loadMask, Cache *cache)
    : type_{type}, cache_{cache}, loadMask_{loadMask} {
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
    if (cache)
      cache->addObject(templateFile_);
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

static std::shared_ptr<CachedProgramObject>
getCachedProgramObject(Cache *cache, const char *hashstr) {
  if (cache) {
    if (auto cachedProg =
            cache->getObjectOfType<CachedProgramObject>(hashstr)) {
      return cachedProg;
    }
  }
  return nullptr;
}

static std::shared_ptr<GraphicsPipelineState>
createGraphicsPipelineState(const char *name, const GraphicsPipelineDesc &desc,
                            Cache *cache = nullptr) {
  // compute hash
  MD5Hasher hasher;
  if (desc.vertexShader)
    hasher.update(desc.vertexShader, std::strlen(desc.vertexShader));
  if (desc.fragmentShader)
    hasher.update(desc.fragmentShader, std::strlen(desc.fragmentShader));
  if (desc.tessControlShader)
    hasher.update(desc.tessControlShader, std::strlen(desc.tessControlShader));
  if (desc.tessEvalShader)
    hasher.update(desc.tessEvalShader, std::strlen(desc.tessEvalShader));
  if (desc.geometryShader)
    hasher.update(desc.geometryShader, std::strlen(desc.geometryShader));
  MD5Hasher::Hash h;
  hasher.finalize(h);
  auto hashstr = "$graphics" + MD5Hasher::hashToString(h);

  auto state = std::make_shared<GraphicsPipelineState>(name);
  state->rasterizerState = desc.rasterizerState;
  state->blendStates = desc.blendStates;
  state->depthStencilState = desc.depthStencilState;
  // init VAO
  state->vao.initialize(desc.inputLayout);

  // try to find a matching shader in the cache
  state->cachedProgram = getCachedProgramObject(cache, hashstr.c_str());

  if (!state->cachedProgram) {
    // create program
    auto prog = ProgramObject::create(
        desc.vertexShader, desc.fragmentShader, desc.geometryShader,
        desc.tessControlShader, desc.tessEvalShader);

    if (!prog.getLinkStatus()) {
      errorMessage("Failed to create program");
      debugMessage("====== Vertex shader ({}): ======\n {}",
                   desc.vertexShaderPath, desc.vertexShader);
      debugMessage("====== Fragment shader ({}): ======\n {}",
                   desc.fragmentShaderPath, desc.fragmentShader);
      return nullptr;
    } else {
      state->cachedProgram = std::make_shared<CachedProgramObject>(
          hashstr.c_str(), std::move(prog));
      if (cache) {
        AG_DEBUG("Adding program {} to cache", hashstr);
        cache->addObject(state->cachedProgram);
      }
    }
  }
  else {
	  AG_DEBUG("Reusing cached program {}", hashstr);
  }

  return state;
}

static std::shared_ptr<ComputePipelineState>
createComputePipelineState(const char *name, const ComputePipelineDesc &desc,
                           Cache *cache = nullptr) {
  // compute hash
  MD5Hasher hasher;
  if (desc.computeShader)
    hasher.update(desc.computeShader, std::strlen(desc.computeShader));
  MD5Hasher::Hash h;
  hasher.finalize(h);
  auto hashstr = "$compute_" + MD5Hasher::hashToString(h);
  auto state = std::make_shared<ComputePipelineState>(name);

  state->cachedProgram = getCachedProgramObject(cache, hashstr.c_str());

  if (!state->cachedProgram) {
    auto prog = ProgramObject::createCompute(desc.computeShader);
    if (!prog.getLinkStatus()) {
      errorMessage("Failed to create program");
      debugMessage("====== Compute shader ({}): ======\n {}",
                   desc.computeShaderPath, desc.computeShader);
      return nullptr;
    } else {
      state->cachedProgram = std::make_shared<CachedProgramObject>(
          hashstr.c_str(), std::move(prog));
      if (cache) {
        AG_DEBUG("Adding program {} to cache", hashstr);
        cache->addObject(state->cachedProgram);
      }
    }
  }
  else {
	  AG_DEBUG("Reusing cached program {}", hashstr);
  }
  return state;
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
