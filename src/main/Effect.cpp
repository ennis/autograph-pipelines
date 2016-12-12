#include "Effect.h"
#include <autograph/support/Debug.h>
#include <iostream>

namespace ag {

//////////////////////////////////////////////////////////////////
// PassBuilder
void PassBuilder::bindTextureInternal(Pass& pass, int slot, GLuint texobj) {
  pass.resources_.textures[slot] = texobj;
}

void PassBuilder::bindTextureImageInternal(Pass& pass, int slot, GLuint texobj) {
  pass.resources_.images[slot] = texobj;
}

void PassBuilder::bindSamplerInternal(Pass& pass, int slot, GLuint samplerobj) {
  pass.resources_.samplers[slot] = samplerobj;
}

void PassBuilder::bindUniformBufferInternal(Pass& pass, int slot,
                                    const ag::gl::BufferSlice &slice) {
  pass.resources_.uniformBuffers[slot] = slice.obj;
  pass.resources_.uniformBufferOffsets[slot] = slice.offset;
  pass.resources_.uniformBufferSizes[slot] = slice.size;
}

void PassBuilder::bindShaderStorageBufferInternal(Pass& pass, int slot,
                                          const ag::gl::BufferSlice &slice) {
  pass.resources_.shaderStorageBuffers[slot] = slice.obj;
  pass.resources_.shaderStorageBufferOffsets[slot] = slice.offset;
  pass.resources_.shaderStorageBufferSizes[slot] = slice.size;
}

void PassBuilder::addDependencyInternal(Pass& pass, Pass &dependency) {
  pass.dependencies_.push_back(&dependency);
}

//////////////////////////////////////////////////////////////////
// DrawPassBuilder
DrawPassBuilder::DrawPassBuilder() 
{
}

void DrawPassBuilder::bindVertexArray(GLuint vao) {
}
void DrawPassBuilder::bindColorBuffer(int index, GLuint texobj) {
  drawPass_.colorBuffers_[index] = texobj;
}
void DrawPassBuilder::bindDepthBuffer(GLuint texobj) {
  drawPass_.depthBuffer_ = texobj;
}
void DrawPassBuilder::bindVertexBuffer(int slot,
                                       const ag::gl::BufferSlice &slice,
                                       int stride) {
  drawPass_.drawResources_.vertexBuffers[slot] = slice.obj;
  drawPass_.drawResources_.vertexBufferOffsets[slot] = slice.offset;
  drawPass_.drawResources_.vertexBufferStrides[slot] = stride;
}

void DrawPassBuilder::setVertexShader(std::string vs) { 
  drawPass_.VS_ = std::move(vs); }

void DrawPassBuilder::setFragmentShader(std::string fs) { 
  drawPass_.FS_ = std::move(fs); }

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

void DrawPassBuilder::setBlendState(int index, const gl::BlendState& blendState) 
{
  drawPass_.blendStates_[index] = blendState;
}

void DrawPassBuilder::setViewport(int index, float x, float y, float w, float h) {
  auto& vp = drawPass_.viewports_[index];
  vp.x = x;
  vp.y = y;
  vp.w = w;
  vp.h = h;
}

void DrawPassBuilder::setRasterizerState(const gl::RasterizerState& rs)  {
  drawPass_.rasterizerState_ = rs;
}

void DrawPassBuilder::setDepthStencilState(const gl::DepthStencilState& ds) {
  drawPass_.depthStencilState_ = ds;
}


/////////////////////////////////////////////////
void DrawPass::compile() {
  prog_ = gl::Program::create(VS_.c_str(), FS_.c_str());
  if (!prog_.getLinkStatus()) {
    errorMessage("Failed to create program for draw pass");
  } else {
    AG_DEBUG("Successfully compiled DrawPass");
  }
}

}
