#include "Effect.h"
#include <autograph/support/Debug.h>
#include <iostream>

namespace ag {

//////////////////////////////////////////////////////////////////
// PassBuilder
void PassBuilder::bindTexture(int slot, GLuint texobj)
{
	pass_->resources_.textures[slot] = texobj;
}

void PassBuilder::bindTextureImage(int slot, GLuint texobj)
{
	pass_->resources_.images[slot] = texobj;
}

void PassBuilder::bindSampler(int slot, GLuint samplerobj) {
	pass_->resources_.samplers[slot] = samplerobj;
}

void PassBuilder::bindUniformBuffer(int slot,
                                    const ag::gl::BufferSlice &slice) 
{
	pass_->resources_.uniformBuffers[slot] = slice.obj;
	pass_->resources_.uniformBufferOffsets[slot] = slice.offset;
	pass_->resources_.uniformBufferSizes[slot] = slice.size;
}

void PassBuilder::bindShaderStorageBuffer(int slot,
                                          const ag::gl::BufferSlice &slice) 
{
	pass_->resources_.shaderStorageBuffers[slot] = slice.obj;
	pass_->resources_.shaderStorageBufferOffsets[slot] = slice.offset;
	pass_->resources_.shaderStorageBufferSizes[slot] = slice.size;
}


//////////////////////////////////////////////////////////////////
// DrawPassBuilder
void DrawPassBuilder::bindVertexArray(GLuint vao)
{
	getPassPtr()->drawResources_.vao = vao;
}
void DrawPassBuilder::bindColorBuffer(int index, GLuint texobj)
{
	getPassPtr()->colorBuffers_[index] = texobj;
}
void DrawPassBuilder::bindDepthBuffer(GLuint texobj)
{
	getPassPtr()->depthBuffer_ = texobj;
}
void DrawPassBuilder::bindVertexBuffer(int slot, const ag::gl::BufferSlice & slice, int stride)
{
	getPassPtr()->drawResources_.vertexBuffers[slot] = slice.obj;
	getPassPtr()->drawResources_.vertexBufferOffsets[slot] = slice.offset;
	getPassPtr()->drawResources_.vertexBufferStrides[slot] = stride;
}
void DrawPassBuilder::addDependency(ag::Pass * dependency)
{
}
void DrawPassBuilder::setVertexShader(std::string vs)
{
}
void DrawPassBuilder::setFragmentShader(std::string fs)
{
}
void DrawPassBuilder::addShaderDef(std::string kw, std::string def)
{
}
void DrawPassBuilder::addShaderKeyword(std::string kw)
{
}
auto DrawPassBuilder::makeDrawPass() -> std::unique_ptr<DrawPass>
{
	return std::unique_ptr<DrawPass>();
}

/////////////////////////////////////////////////
void DrawPass::compile()
{
	// Don't preprocess here: it should be done manully
	prog_ = gl::Program::create(VS_.c_str(), FS_.c_str());
	if (prog_.getLinkStatus()) {
		errorMessage("Failed to create program for draw pass");
	}
	// TODO do some checking here		
}

/////////////////////////////////////////////////
Pipeline::Pipeline()
{ initialize(); }

Pipeline::~Pipeline() {}

void Pipeline::initialize() {
}

void Pipeline::reset() {
  resources.clear();
  passes.clear();
  //L = {};
  initialize();
}

/*void Pipeline::addScriptPath(const char *path) {
  std::string package_path = L["package"]["path"];
  L["package"]["path"] =
      package_path + (!package_path.empty() ? ";" : "") + path + "?.lua";
}

void Pipeline::loadScript(const char *path) {
  try {
    L.script_file(path);
  } catch (const sol::error &e) {
    errorMessage("{}", e.what());
  }
}*/

gl::Texture *Pipeline::createTexture(const ImageDesc &desc) {
  auto &a = resources[name];
  if (!a) {
    // no texture with this name yet, create one
    a = TextureResource{std::make_unique<gl::Texture>(desc)};
    // debug info
    if (desc.dimensions == ImageDimensions::Image2D) {
      AG_DEBUG("createTexture2D {} {}x{} {}, {} mip level(s)", name, desc.width,
               desc.height, getImageFormatInfo(desc.format).name,
               desc.numMipmaps);
    } else if (desc.dimensions == ImageDimensions::Image3D) {
      AG_DEBUG("createTexture3D {} {}x{}x{} {}, {} mip level(s)", name,
               desc.width, desc.height, desc.depth,
               getImageFormatInfo(desc.format).name, desc.numMipmaps);
    }
  } else {
    warningMessage("{}: a resource with the same name already exists", name);
  }
  if (auto p = a.target<TextureResource>()) {
    return p->tex.get();
  } else {
    return nullptr;
  }
}

gl::Texture *Pipeline::createTexture2D(const char *name, ImageFormat fmt, int w,
                                       int h, int numMips) {
  ImageDesc desc;
  desc.format = fmt;
  desc.dimensions = ImageDimensions::Image2D;
  desc.width = w;
  desc.height = h;
  desc.depth = 1;
  desc.numMipmaps = numMips;
  return createTexture(name, desc);
}

gl::Texture *Pipeline::createTexture3D(const char *name, ImageFormat fmt, int w,
                                       int h, int d, int numMips) {
  ImageDesc desc;
  desc.format = fmt;
  desc.dimensions = ImageDimensions::Image2D;
  desc.width = w;
  desc.height = h;
  desc.depth = d;
  desc.numMipmaps = numMips;
  return createTexture(name, desc);
}

/*gl::Texture *Pipeline::getTexture(const char *name) {
  auto match = textures.find(name);
  if (match == textures.end()) {
    return nullptr;
  }
  return match->second.get();
}*/

auto Pipeline::getPass(const char *name) -> Pass * {
  auto match = passes.find(name);
  if (match == passes.end()) {
    return nullptr;
  }
  return match->second.get();
}

auto Pipeline::createComputePass(const char *name) -> ComputePass * {
  auto &p = passes[name];
  if (!p) {
    // no texture with this name yet, create one
    p = std::make_unique<ComputePass>();
  } else {
    warningMessage("{}: a pass with the same name already exists", name);
  }
  return static_cast<ComputePass *>(p.get());
}

auto Pipeline::createSampler(const char *name, const gl::SamplerDesc &desc)
    -> gl::Sampler * {
  auto &a = resources[name];
  if (!a) {
    // no texture with this name yet, create one
    a = SamplerResource{std::make_unique<gl::Sampler>(desc)};
    AG_DEBUG("Sampler {}", name);
  } else {
    warningMessage("{}: a resource with the same name already exists", name);
  }
  if (auto p = a.target<SamplerResource>()) {
    return p->sampler.get();
  } else {
    return nullptr;
  }
}

// Lua API
AG_LUA_API auto Pipeline_CreateTexture2D(ag::Pipeline *pPipeline,
                                         const char *name,
                                         ag::ImageFormat imgFmt, int width,
                                         int height, int numMips)
    -> ag::gl::Texture *;
AG_LUA_API auto Pipeline_CreateTexture3D(ag::Pipeline *pPipeline,
                                         const char *name,
                                         ag::ImageFormat imgFmt, int width,
                                         int height, int depth, int numMips)
    -> ag::gl::Texture *;

AG_LUA_API auto Pipeline_CreateSampler(ag::Pipeline *pPipeline,
                                       const char *name,
                                       const ag::gl::SamplerDesc *desc)
    -> ag::gl::Sampler *;

AG_LUA_API auto Pipeline_CreateComputePass(ag::Pipeline *pPipeline,
                                           const char *name)
    -> ag::ComputePass *;
AG_LUA_API auto Pipeline_CreateScreenPass(ag::Pipeline *pPipeline,
                                          const char *name)
    -> ag::ScreenPass *;
AG_LUA_API auto Pipeline_CreateDrawPass(ag::Pipeline *pPipeline,
                                        const char *name) -> ag::DrawPass *;
AG_LUA_API auto Pipeline_GenerateTextureID() -> int;

AG_LUA_API void Pass_BindTexture(ag::Pass *pass, int slot,
                                 ag::gl::Texture *tex);
AG_LUA_API void Pass_BindTextureImage(ag::Pass *pass, int slot,
                                      ag::gl::Texture *tex);
AG_LUA_API void Pass_BindSampler(ag::Pass *pass, int slot,
                                 ag::gl::Sampler *sampler);
AG_LUA_API void Pass_AddDependency(ag::Pass *pass,
                                   ag::Pass *dependency);

AG_LUA_API void DrawPass_BindColorBuffer(ag::DrawPass* pass, int index, ag::gl::Texture* tex);
AG_LUA_API void DrawPass_BindDepthBuffer(ag::DrawPass* pass, ag::gl::Texture* tex);
AG_LUA_API auto DrawPass_GetRasterizerState(ag::DrawPass* pass) -> ag::gl::RasterizerState*;
AG_LUA_API auto DrawPass_GetDepthStencilState(ag::DrawPass* pass) -> ag::gl::DepthStencilState*;
AG_LUA_API auto DrawPass_GetBlendState(ag::DrawPass* pass, int index) -> ag::gl::BlendState*;
AG_LUA_API auto DrawPass_GetViewport(ag::DrawPass* pass, int index) -> ag::gl::Viewport*;

/////////////////////////////////////////////////
AG_LUA_API ag::gl::Texture *FXCreateTexture2D(ag::Pipeline *pPipeline,
                                              const char *name,
                                              ag::ImageFormat imgFmt, int width,
                                              int height, int numMips) {
  return pPipeline->createTexture2D(name, imgFmt, width, height, numMips);
}

/////////////////////////////////////////////////
AG_LUA_API ag::gl::Texture *FXCreateTexture3D(ag::Pipeline *pPipeline,
                                              const char *name,
                                              ag::ImageFormat imgFmt, int width,
                                              int height, int depth,
                                              int numMips) {
  return pPipeline->createTexture3D(name, imgFmt, width, height, depth,
                                    numMips);
}

AG_LUA_API ag::ComputePass *FXCreateComputePass(ag::Pipeline *pPipeline,
                                                    const char *name) {
  return nullptr;
}

AG_LUA_API ag::ScreenPass *FXCreateScreenPass(ag::Pipeline *pPipeline,
                                                  const char *name) {
  return nullptr;
}

AG_LUA_API ag::DrawPass *FXCreateDrawPass(ag::Pipeline *pPipeline,
                                              const char *name) {
  return nullptr;
}

/////////////////////////////////////////////////
AG_LUA_API ag::gl::Sampler *FXCreateSampler(ag::Pipeline *pPipeline,
                                            const char *name,
                                            const ag::gl::SamplerDesc *desc) {
  return pPipeline->createSampler(name, *desc);
}

/////////////////////////////////////////////////
AG_LUA_API void FXBindTexture(ag::Pipeline *pPipeline, ag::Pass *pass,
                              int slot, ag::gl::Texture *tex) {}

/////////////////////////////////////////////////
AG_LUA_API void FXBindTextureImage(ag::Pipeline *pPipeline,
                                   ag::Pass *pass, int slot,
                                   ag::gl::Texture *tex) {}

/////////////////////////////////////////////////
AG_LUA_API void FXBindSampler(ag::Pipeline *pPipeline, ag::Pass *pass,
                              int slot, ag::gl::Sampler *sampler) {}

/////////////////////////////////////////////////
AG_LUA_API int FXGenerateTextureID() {
  static int currentId = 0;
  return currentId++;
}

/////////////////////////////////////////////////
AG_LUA_API void FXAddPassDependency(ag::Pipeline *pPipeline,
                                    ag::Pass *pass,
                                    ag::Pass *dependency) {}


}