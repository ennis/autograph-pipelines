#include <autograph/engine/Shader.h>
#include <autograph/gl/VertexArray.h>
#include <autograph/support/Debug.h>
#include <iostream>

namespace ag {

//////////////////////////////////////////////////////////////////
// PassBuilder
void PassBuilder::bindTextureInternal(Pass &pass, int slot, GLuint texobj) {
  // pass.resources_.textures[slot] = texobj;
}

void PassBuilder::bindTextureImageInternal(Pass &pass, int slot,
                                           GLuint texobj) {
  // pass.resources_.images[slot] = texobj;
}

void PassBuilder::bindSamplerInternal(Pass &pass, int slot, GLuint samplerobj) {
  // pass.resources_.samplers[slot] = samplerobj;
}

void PassBuilder::bindUniformBufferInternal(Pass &pass, int slot,
                                            const ag::gl::BufferSlice &slice) {
  /*pass.resources_.uniformBuffers[slot] = slice.obj;
  pass.resources_.uniformBufferOffsets[slot] = slice.offset;
  pass.resources_.uniformBufferSizes[slot] = slice.size;*/
}

void PassBuilder::bindShaderStorageBufferInternal(
    Pass &pass, int slot, const ag::gl::BufferSlice &slice) {
  /*pass.resources_.shaderStorageBuffers[slot] = slice.obj;
  pass.resources_.shaderStorageBufferOffsets[slot] = slice.offset;
  pass.resources_.shaderStorageBufferSizes[slot] = slice.size;*/
}

void PassBuilder::addDependencyInternal(Pass &pass, Pass &dependency) {
  // pass.dependencies_.push_back(&dependency);
}

//////////////////////////////////////////////////////////////////
// DrawPassBuilder
DrawPassBuilder::DrawPassBuilder() {}

void DrawPassBuilder::loadFromTable(sol::table config) {
  {
    sol::table blendStates = config["blendState"];
    blendStates.for_each([this](sol::object key, sol::object value) {
      gl::BlendState bs;
      assert(key.get_type() == sol::type::number);
      assert(value.get_type() == sol::type::table);
      auto index = key.as<int>();
      auto table = value.as<sol::table>();
      bs.enabled = table.get_or("enabled", false);
      if (bs.enabled) {
        bs.modeRGB = table.get_or("modeRGB", GL_FUNC_ADD);
        bs.modeAlpha = table.get_or("modeAlpha", GL_FUNC_ADD);
        bs.funcSrcRGB = table.get_or("funcSrcRGB", GL_SRC_ALPHA);
        bs.funcDstRGB = table.get_or("funcDstRGB", GL_ONE_MINUS_SRC_ALPHA);
        bs.funcSrcAlpha = table.get_or("funcSrcAlpha", GL_ONE);
        bs.funcDstAlpha = table.get_or("funcDstAlpha", GL_ZERO);
      }
      this->setBlendState(index, bs);
    });
  }

  {
    sol::table table = config["rasterizerState"];
    gl::RasterizerState rs;
    rs.fillMode = table.get_or("fillMode", GL_FILL);
    rs.cullMode = table.get_or("cullMode", GL_NONE);
    rs.frontFace = table.get_or("frontFace", GL_CCW);
    rs.depthBias = table.get_or("depthBias", 1.0f);
    rs.slopeScaledDepthBias = table.get_or("slopeScaledDepthBias", 1.0f);
    rs.depthClipEnable = table.get_or("depthClipEnable", false);
    rs.scissorEnable = table.get_or("scissorEnable", false);
    setRasterizerState(rs);
  }

  {
    sol::table table = config["depthStencilState"];
    gl::DepthStencilState dss;
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
    setDepthStencilState(dss);
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
        float x = table.get_or("x", 0.0f);
        float y = table.get_or("y", 0.0f);
        float w = table.get_or("w", 0.0f);
        float h = table.get_or("h", 0.0f);
        this->setViewport(index, x, y, w, h);
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
      attrib.stride = table.get_or("stride", 12);
      attrib.normalized = table.get_or("normalized", false);
      attribs.push_back(attrib);
    });
    drawPass_.vao_.initialize(attribs);
  }

  {
    // shaders
    setVertexShader(config["vertexShader"]);
    setFragmentShader(config["fragmentShader"]);
  }
}

void DrawPassBuilder::bindVertexArray(GLuint vao) {}
void DrawPassBuilder::bindColorBuffer(int index, GLuint texobj) {
  drawPass_.colorBuffers_[index] = texobj;
}
void DrawPassBuilder::bindDepthBuffer(GLuint texobj) {
  drawPass_.depthBuffer_ = texobj;
}
void DrawPassBuilder::bindVertexBuffer(int slot,
                                       const ag::gl::BufferSlice &slice,
                                       int stride) {
  /*drawPass_.drawResources_.vertexBuffers[slot] = slice.obj;
  drawPass_.drawResources_.vertexBufferOffsets[slot] = slice.offset;
  drawPass_.drawResources_.vertexBufferStrides[slot] = stride;*/
}

void DrawPassBuilder::setVertexShader(std::string vs) {
  drawPass_.VS_ = std::move(vs);
}

void DrawPassBuilder::setFragmentShader(std::string fs) {
  drawPass_.FS_ = std::move(fs);
}

void DrawPassBuilder::addShaderDef(std::string kw, std::string def) {
  // TODO
}

void DrawPassBuilder::addShaderKeyword(std::string kw) {
  // TODO
}

auto DrawPassBuilder::makeDrawPass() -> std::unique_ptr<DrawPass> {
  // copy-construct a new DrawPass
  auto newpass = std::make_unique<DrawPass>(std::move(drawPass_));
  newpass->compile();
  return newpass;
}

void DrawPassBuilder::setBlendState(int index,
                                    const gl::BlendState &blendState) {
  drawPass_.drawStates_.blendStates[index] = blendState;
}

void DrawPassBuilder::setViewport(int index, float x, float y, float w,
                                  float h) {
  auto &vp = drawPass_.drawStates_.viewports[index];
  vp.x = x;
  vp.y = y;
  vp.w = w;
  vp.h = h;
}

void DrawPassBuilder::setRasterizerState(const gl::RasterizerState &rs) {
  drawPass_.drawStates_.rasterizerState = rs;
}

void DrawPassBuilder::setDepthStencilState(const gl::DepthStencilState &ds) {
  drawPass_.drawStates_.depthStencilState = ds;
}

//////////////////////////////////////////////
namespace detail
{
	ScriptContext& ensureShaderLuaStateInitialized()
	{
		static ScriptContext sc;
		static bool initialized = false;
		if (!initialized) {
			sc.script("require 'shader_utils'");	// Load shader utils
			initialized = true;
		}
		return sc;
	}
}

//////////////////////////////////////////////
static void loadShaderFile(const char *shaderId) {
	auto& L = detail::ensureShaderLuaStateInitialized();
	AG_DEBUG("loadShaderFile: {}", shaderId);
	L.script(std::string{ "require '" } +shaderId + "'");
}

/////////////////////////////////////////////////
void DrawPass::initialize(const char *shaderId, sol::table table)
{
	auto &L = detail::ensureShaderLuaStateInitialized();
	std::string shaderIdStr{ shaderId };
	auto p = shaderIdStr.find_last_of(':');
	if (p == std::string::npos) {
		return;
	}
	auto shaderFileId = shaderIdStr.substr(0, p);
	auto passId = shaderIdStr.substr(p + 1);
	AG_DEBUG("createDrawPassInternal: {}", shaderId);
	loadShaderFile(shaderFileId.c_str());
	try {
		auto config = L["shader_utils"]["createShaderFromTemplate"](passId, table);
		DrawPassBuilder builder;
		builder.loadFromTable(config);
		// move construct into this
		// TODO this is hackish
		*this = std::move(*builder.makeDrawPass());
	}
	catch (sol::error &e) {
		errorMessage("Error loading shader pass {}:\n\t{}", passId, e.what());
		throw;
	}
}

/////////////////////////////////////////////////
void DrawPass::compile() {
  prog_ = gl::Program::create(VS_.c_str(), FS_.c_str());
  if (!prog_.getLinkStatus()) {
    errorMessage("Failed to create program for draw pass");
    debugMessage("====== Vertex shader: ======\n {}", VS_.c_str());
    debugMessage("====== Fragment shader: ======\n {}", FS_.c_str());
  } else {
    // AG_DEBUG("Successfully compiled DrawPass");
  }
  drawStates_.program = prog_.object();
  drawStates_.vertexArray = vao_.object();
}



}
