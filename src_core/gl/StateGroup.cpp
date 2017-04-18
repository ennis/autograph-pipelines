#include <autograph/gl/StateGroup.h>
#include <autograph/gl/Capture.h>
#include <autograph/gl/gl_core_4_5.h>

namespace ag {

static void bindUniforms(const Uniforms &uniforms) {
  // VBOs
  for (unsigned i = 0; i < kMaxVertexBufferSlots; ++i)
    if (uniforms.vertexBuffers[i])
      gl::BindVertexBuffer(i, uniforms.vertexBuffers[i],
                         uniforms.vertexBufferOffsets[i],
                         uniforms.vertexBufferStrides[i]);
    else
      gl::BindVertexBuffer(i, 0, 0, 0);
  // IBO
  if (uniforms.indexBuffer.obj)
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, uniforms.indexBuffer.obj);
  // Textures
  gl::BindTextures(0, kMaxTextureUnits, uniforms.textures.data());
  // Samplers
  gl::BindSamplers(0, kMaxTextureUnits, uniforms.samplers.data());
  // Images
  gl::BindImageTextures(0, kMaxImageUnits, uniforms.images.data());
  // UBOs
  for (unsigned i = 0; i < kMaxUniformBufferSlots; ++i) {
    if (uniforms.uniformBuffers[i])
      gl::BindBufferRange(gl::UNIFORM_BUFFER, i, uniforms.uniformBuffers[i],
                        uniforms.uniformBufferOffsets[i],
                        uniforms.uniformBufferSizes[i]);
    else
      gl::BindBufferBase(gl::UNIFORM_BUFFER, i, 0);
  }
  // SSBOs
  for (unsigned i = 0; i < kMaxUniformBufferSlots; ++i) {
    if (uniforms.uniformBuffers[i])
      gl::BindBufferRange(gl::SHADER_STORAGE_BUFFER, i,
                        uniforms.shaderStorageBuffers[i],
                        uniforms.shaderStorageBufferOffsets[i],
                        uniforms.shaderStorageBufferSizes[i]);
    else
      gl::BindBufferBase(gl::SHADER_STORAGE_BUFFER, i, 0);
  }
}

void bindStateGroup(const StateGroup &sg) {
  // Viewports
  if (not_empty(sg.mask & StateGroupMask::Viewports)) {
    gl::ViewportArrayv(0, 8, (const float *)sg.drawStates.viewports.data());
  }

  // Scissor rect
  if (not_empty(sg.mask & StateGroupMask::ScissorRect)) {
    gl::ScissorArrayv(0, 8, (const int *)sg.drawStates.scissorRects.data());
  }

  // Blend states
  if (not_empty(sg.mask & StateGroupMask::BlendStates)) {
    if (sg.drawStates.blendStates.empty())
      gl::Disable(gl::BLEND);
    else {
      gl::Enable(gl::BLEND); // XXX is this necessary
      for (int i = 0; i < 8; ++i) {
        if (sg.drawStates.blendStates[i].enabled) {
          gl::Enablei(gl::BLEND, i);
          gl::BlendEquationSeparatei(i, sg.drawStates.blendStates[i].modeRGB,
                                   sg.drawStates.blendStates[i].modeAlpha);
          gl::BlendFuncSeparatei(i, sg.drawStates.blendStates[i].funcSrcRGB,
                               sg.drawStates.blendStates[i].funcDstRGB,
                               sg.drawStates.blendStates[i].funcSrcAlpha,
                               sg.drawStates.blendStates[i].funcDstAlpha);
        } else
          gl::Disablei(gl::BLEND, i);
      }
    }
  }

  // Depth stencil state
  if (not_empty(sg.mask & StateGroupMask::DepthStencilState)) {
    if (sg.drawStates.depthStencilState.depthTestEnable)
      gl::Enable(gl::DEPTH_TEST);
    else
      gl::Disable(gl::DEPTH_TEST);

    if (sg.drawStates.depthStencilState.depthWriteEnable)
      gl::DepthMask(gl::TRUE_);
    else
      gl::DepthMask(gl::FALSE_);

    gl::DepthFunc(sg.drawStates.depthStencilState.depthTestFunc);

    if (sg.drawStates.depthStencilState.stencilEnable) {
      gl::Enable(gl::STENCIL_TEST);
      gl::StencilFuncSeparate(sg.drawStates.depthStencilState.stencilFace,
                            sg.drawStates.depthStencilState.stencilFunc,
                            sg.drawStates.depthStencilState.stencilRef,
                            sg.drawStates.depthStencilState.stencilMask);
      gl::StencilOp(sg.drawStates.depthStencilState.stencilOpSfail,
                  sg.drawStates.depthStencilState.stencilOpDPFail,
                  sg.drawStates.depthStencilState.stencilOpDPPass);
    } else
      gl::Disable(gl::STENCIL_TEST);
  }

  // Rasterizer
  if (not_empty(sg.mask & StateGroupMask::RasterizerState)) {
    gl::PolygonMode(gl::FRONT_AND_BACK, sg.drawStates.rasterizerState.fillMode);
    gl::Disable(gl::CULL_FACE);
  }

  // Vertex array
  if (not_empty(sg.mask & StateGroupMask::VertexArray)) {
    gl::BindVertexArray(sg.drawStates.vertexArray);
  }

  // program
  if (not_empty(sg.mask & StateGroupMask::Program)) {
    gl::UseProgram(sg.drawStates.program);
  }

  // Uniforms
  bindUniforms(sg.uniforms);
}

// Bind state group to GL pipeline
// void StateGroup::operator()(GLBindContext &bindContext) {
/*if (mask & StateGroupMask::Viewports) {
  for (auto &&vp : viewports) {
        ViewportIndexedf(vp.slot, vp.x, vp.y, vp.width, vp.height);
  }
}

if (mask & StateGroupMask::BlendStates) {
  if (blendStates.empty())
        gl::Disable(gl::BLEND);
  else {
        gl::Enable(gl::BLEND); // XXX is this necessary
        for (auto &&bs : blendStates) {
          if (bs.state.enabled) {
                gl::Enablei(gl::BLEND, bs.slot);
                gl::BlendEquationSeparatei(bs.slot, bs.state.modeRGB,
                                                                 bs.state.modeAlpha);
                gl::BlendFuncSeparatei(bs.slot, bs.state.funcSrcRGB,
                                                         bs.state.funcDstRGB,
bs.state.funcSrcAlpha,
                                                         bs.state.funcDstAlpha);
          } else
                gl::Disablei(gl::BLEND, bs.slot);
        }
  }
}

if (mask & StateGroupMask::DepthStencilState))
{
        if (depthStencilState.depthTestEnable)
          gl::Enable(gl::DEPTH_TEST);
        else
          gl::Disable(gl::DEPTH_TEST);

        gl::DepthFunc(depthStencilState.depthTestFunc);

        if (depthStencilState.stencilEnable) {
          gl::Enable(gl::STENCIL_TEST);
          gl::StencilFuncSeparate(
                  depthStencilState.stencilFace, depthStencilState.stencilFunc,
                  depthStencilState.stencilRef, depthStencilState.stencilMask);
          gl::StencilOp(depthStencilState.stencilOpSfail,
                                  depthStencilState.stencilOpDPFail,
                                  depthStencilState.stencilOpDPPass);
        } else
          gl::Disable(gl::STENCIL_TEST);
  }

if (mask & StateGroupMask::rasterizerState) {
  gl::PolygonMode(gl::FRONT_AND_BACK, rasterizerState.fillMode);
  gl::Disable(gl::CULL_FACE);
}

if (mask & StateGroupMask::VertexBuffers) {
  for (auto &&vbo : vbos)
        gl::BindVertexBuffer(vbo.slot, vbo.buf.obj, vbo.buf.offset, vbo.stride);
}

if (mask & StateGroupMask::UniformBuffers) {
  for (auto &&ubo : ubos)
        gl::BindBufferRange(gl::UNIFORM_BUFFER, ubo.slot, ubo.buf.obj,
                                          ubo.buf.offset, ubo.buf.size);
}

if (mask & StateGroupMask::ShaderStorageBuffers) {
  for (auto &&ssbo : ssbos)
        gl::BindBufferRange(gl::SHADER_STORAGE_BUFFER, ssbo.slot, ssbo.buf.obj,
                                          ssbo.buf.offset, ssbo.buf.size);
}

if (mask & StateGroupMask::Images) {
  for (auto &&image : images)
        gl::BindImageTextures(image.slot, 1, images.tex->object());
}

if (mask & StateGroupMask::Textures) {
  for (auto &&tex : textures)
        gl::BindTextureUnit(tex.slot, tex.tex->object());
}

if (mask & StateGroupMask::Textures) {
  for (auto &&s : samplers)
        gl::BindSampler(s.slot, s.sampler->object());
}

if (mask & StateGroupMask::IndexBuffer) {
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, indexBuffer.obj);
  bindContext.indexBufferOffset = indexBuffer.offset;
}

if (mask & StateGroupMask::Program)
  gl::UseProgram(prog->object());

if (mask & StateGroupMask::VertexArray)
  gl::BindVertexArray(vao->object());

if (mask & StateGroupMask::Framebuffer)
  gl::BindFramebuffer(fbo->object());*/
//}
}
