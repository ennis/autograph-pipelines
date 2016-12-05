#include "Effect.h"
#include <autograph/support/Debug.h>
#include <iostream>

namespace ag {

//////////////////////////////////////////////////////////////////
// PassBuilder
void PassBuilder::bindTexture(int slot, GLuint texobj) {
  pass_->resources_.textures[slot] = texobj;
}

void PassBuilder::bindTextureImage(int slot, GLuint texobj) {
  pass_->resources_.images[slot] = texobj;
}

void PassBuilder::bindSampler(int slot, GLuint samplerobj) {
  pass_->resources_.samplers[slot] = samplerobj;
}

void PassBuilder::bindUniformBuffer(int slot,
                                    const ag::gl::BufferSlice &slice) {
  pass_->resources_.uniformBuffers[slot] = slice.obj;
  pass_->resources_.uniformBufferOffsets[slot] = slice.offset;
  pass_->resources_.uniformBufferSizes[slot] = slice.size;
}

void PassBuilder::bindShaderStorageBuffer(int slot,
                                          const ag::gl::BufferSlice &slice) {
  pass_->resources_.shaderStorageBuffers[slot] = slice.obj;
  pass_->resources_.shaderStorageBufferOffsets[slot] = slice.offset;
  pass_->resources_.shaderStorageBufferSizes[slot] = slice.size;
}

void PassBuilder::addDependency(ag::Pass *dependency) {
  pass_->dependencies_.push_back(dependency);
}

//////////////////////////////////////////////////////////////////
// DrawPassBuilder
void DrawPassBuilder::bindVertexArray(GLuint vao) {
  getPassPtr()->drawResources_.vao = vao;
}
void DrawPassBuilder::bindColorBuffer(int index, GLuint texobj) {
  getPassPtr()->colorBuffers_[index] = texobj;
}
void DrawPassBuilder::bindDepthBuffer(GLuint texobj) {
  getPassPtr()->depthBuffer_ = texobj;
}
void DrawPassBuilder::bindVertexBuffer(int slot,
                                       const ag::gl::BufferSlice &slice,
                                       int stride) {
  getPassPtr()->drawResources_.vertexBuffers[slot] = slice.obj;
  getPassPtr()->drawResources_.vertexBufferOffsets[slot] = slice.offset;
  getPassPtr()->drawResources_.vertexBufferStrides[slot] = stride;
}

void DrawPassBuilder::setVertexShader(std::string vs) { VS_ = std::move(vs); }

void DrawPassBuilder::setFragmentShader(std::string fs) { FS_ = std::move(fs); }

void DrawPassBuilder::addShaderDef(std::string kw, std::string def) {
  // TODO
}

void DrawPassBuilder::addShaderKeyword(std::string kw) {
  // TODO
}

auto DrawPassBuilder::makeDrawPass() -> std::unique_ptr<DrawPass> {
  auto ptr = this->pass_.release();
  return std::unique_ptr<DrawPass>{static_cast<DrawPass *>(ptr)};
}

/////////////////////////////////////////////////
void DrawPass::compile() {
  // Don't preprocess here: it should be done manully
  prog_ = gl::Program::create(VS_.c_str(), FS_.c_str());
  if (prog_.getLinkStatus()) {
    errorMessage("Failed to create program for draw pass");
  }
  // TODO do some checking here
}

// TODO: autogen
sol::table openLuaModule_GL(sol::this_state s) {
  sol::state_view lua{s};

  sol::table module = lua.create_table();

  module.new_usertype<gl::Texture>(
      "Texture", "create1D", sol::factories(&gl::Texture::create1D), "create2D",
      sol::factories(&gl::Texture::create2D), "width",
      sol::property(&gl::Texture::width), "height",
      sol::property(&gl::Texture::height), "format",
      sol::property(&gl::Texture::format), "object",
      sol::property(&gl::Texture::object),
              "reset", &gl::Texture::reset);

  /*module["ImageFormat"] = lua.create_table_with(
      "RGBA32_Float", ImageFormat::RGBA32_Float,
      "RGBA16_Float", ImageFormat::RGBA16_Float,
      "RG32_Float", ImageFormat::RG32_Float,
      "R32_Float", ImageFormat::R32_Float,
      "R8_Unorm", ImageFormat::R8_Unorm,
      "RGBA8_Unorm", ImageFormat::RGBA8_Unorm,
      "RGBA8_Snorm", ImageFormat::RGBA8_Snorm,
      "RGB_11_11_10_Float_Packed", ImageFormat::RGB_11_11_10_Float_Packed,
      "Depth32_Float", ImageFormat::Depth32_Float);*/

  return module;
}

/*void registerLuaUsertypes(sol::state& lua)
{
    // DrawPassBuilder
    lua.new_usertype<DrawPassBuilder>("DrawPassBuilder",
                                      sol::constructors<sol::types<>>(),
                                      "bindVertexArray",
&DrawPassBuilder::bindVertexArray,
                                      "bindColorBuffer",
&DrawPassBuilder::bindColorBuffer,
                                      "bindDepthBuffer",
&DrawPassBuilder::bindDepthBuffer,
                                      "bindVertexBuffer",
&DrawPassBuilder::bindVertexBuffer,
                                      "addDependency",
&DrawPassBuilder::addDependency,
                                      "setVertexShader",
&DrawPassBuilder::setVertexShader,
                                      "setFragmentShader",
&DrawPassBuilder::setFragmentShader,
                                      "addShaderKeyword",
&DrawPassBuilder::addShaderKeyword,
                                      "addShaderDef",
&DrawPassBuilder::addShaderDef
                                      );

}*/
}
