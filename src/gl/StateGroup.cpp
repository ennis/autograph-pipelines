#include <autograph/gl/StateGroup.h>
#include <autograph/gl/Capture.h>
#include <gl_core_4_5.h>

namespace ag {
namespace gl {

static void bindUniforms(const Uniforms &uniforms) {
  // VBOs
  for (unsigned i = 0; i < kMaxVertexBufferSlots; ++i)
    if (uniforms.vertexBuffers[i])
      glBindVertexBuffer(i, uniforms.vertexBuffers[i],
                         uniforms.vertexBufferOffsets[i],
                         uniforms.vertexBufferStrides[i]);
    else
      glBindVertexBuffer(i, 0, 0, 0);
  // IBO
  if (uniforms.indexBuffer.obj)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniforms.indexBuffer.obj);
  // Textures
  glBindTextures(0, kMaxTextureUnits, uniforms.textures.data());
  // Samplers
  glBindSamplers(0, kMaxTextureUnits, uniforms.samplers.data());
  // Images
  glBindImageTextures(0, kMaxImageUnits, uniforms.images.data());
  // UBOs
  for (unsigned i = 0; i < kMaxUniformBufferSlots; ++i) {
    if (uniforms.uniformBuffers[i])
      glBindBufferRange(GL_UNIFORM_BUFFER, i, uniforms.uniformBuffers[i],
                        uniforms.uniformBufferOffsets[i],
                        uniforms.uniformBufferSizes[i]);
    else
      glBindBufferBase(GL_UNIFORM_BUFFER, i, 0);
  }
  // SSBOs
  for (unsigned i = 0; i < kMaxUniformBufferSlots; ++i) {
    if (uniforms.uniformBuffers[i])
      glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i,
                        uniforms.shaderStorageBuffers[i],
                        uniforms.shaderStorageBufferOffsets[i],
                        uniforms.shaderStorageBufferSizes[i]);
    else
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, 0);
  }
}

void bindStateGroup(const StateGroup &sg) {
  // Viewports
  if (not_empty(sg.mask & StateGroupMask::Viewports)) {
    glViewportArrayv(0, 8, (const float *)sg.drawStates.viewports.data());
  }

  // Scissor rect
  if (not_empty(sg.mask & StateGroupMask::ScissorRect)) {
    glScissorArrayv(0, 8, (const int *)sg.drawStates.scissorRects.data());
  }

  // Blend states
  if (not_empty(sg.mask & StateGroupMask::BlendStates)) {
    if (sg.drawStates.blendStates.empty())
      glDisable(GL_BLEND);
    else {
      glEnable(GL_BLEND); // XXX is this necessary
      for (int i = 0; i < 8; ++i) {
        if (sg.drawStates.blendStates[i].enabled) {
          glEnablei(GL_BLEND, i);
          glBlendEquationSeparatei(i, sg.drawStates.blendStates[i].modeRGB,
                                   sg.drawStates.blendStates[i].modeAlpha);
          glBlendFuncSeparatei(i, sg.drawStates.blendStates[i].funcSrcRGB,
                               sg.drawStates.blendStates[i].funcDstRGB,
                               sg.drawStates.blendStates[i].funcSrcAlpha,
                               sg.drawStates.blendStates[i].funcDstAlpha);
        } else
          glDisablei(GL_BLEND, i);
      }
    }
  }

  // Depth stencil state
  if (not_empty(sg.mask & StateGroupMask::DepthStencilState)) {
    if (sg.drawStates.depthStencilState.depthTestEnable)
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);

    if (sg.drawStates.depthStencilState.depthWriteEnable)
      glDepthMask(GL_TRUE);
    else
      glDepthMask(GL_FALSE);

    glDepthFunc(sg.drawStates.depthStencilState.depthTestFunc);

    if (sg.drawStates.depthStencilState.stencilEnable) {
      glEnable(GL_STENCIL_TEST);
      glStencilFuncSeparate(sg.drawStates.depthStencilState.stencilFace,
                            sg.drawStates.depthStencilState.stencilFunc,
                            sg.drawStates.depthStencilState.stencilRef,
                            sg.drawStates.depthStencilState.stencilMask);
      glStencilOp(sg.drawStates.depthStencilState.stencilOpSfail,
                  sg.drawStates.depthStencilState.stencilOpDPFail,
                  sg.drawStates.depthStencilState.stencilOpDPPass);
    } else
      glDisable(GL_STENCIL_TEST);
  }

  // Rasterizer
  if (not_empty(sg.mask & StateGroupMask::RasterizerState)) {
    glPolygonMode(GL_FRONT_AND_BACK, sg.drawStates.rasterizerState.fillMode);
    glDisable(GL_CULL_FACE);
  }

  // Vertex array
  if (not_empty(sg.mask & StateGroupMask::VertexArray)) {
    glBindVertexArray(sg.drawStates.vertexArray);
  }

  // program
  if (not_empty(sg.mask & StateGroupMask::Program)) {
    glUseProgram(sg.drawStates.program);
  }

  // Uniforms
  bindUniforms(sg.uniforms);
}

// Bind state group to GL pipeline
// void StateGroup::operator()(GLBindContext &bindContext) {
/*if (mask & StateGroupMask::Viewports) {
  for (auto &&vp : viewports) {
        glViewportIndexedf(vp.slot, vp.x, vp.y, vp.width, vp.height);
  }
}

if (mask & StateGroupMask::BlendStates) {
  if (blendStates.empty())
        glDisable(GL_BLEND);
  else {
        glEnable(GL_BLEND); // XXX is this necessary
        for (auto &&bs : blendStates) {
          if (bs.state.enabled) {
                glEnablei(GL_BLEND, bs.slot);
                glBlendEquationSeparatei(bs.slot, bs.state.modeRGB,
                                                                 bs.state.modeAlpha);
                glBlendFuncSeparatei(bs.slot, bs.state.funcSrcRGB,
                                                         bs.state.funcDstRGB,
bs.state.funcSrcAlpha,
                                                         bs.state.funcDstAlpha);
          } else
                glDisablei(GL_BLEND, bs.slot);
        }
  }
}

if (mask & StateGroupMask::DepthStencilState))
{
        if (depthStencilState.depthTestEnable)
          glEnable(GL_DEPTH_TEST);
        else
          glDisable(GL_DEPTH_TEST);

        glDepthFunc(depthStencilState.depthTestFunc);

        if (depthStencilState.stencilEnable) {
          glEnable(GL_STENCIL_TEST);
          glStencilFuncSeparate(
                  depthStencilState.stencilFace, depthStencilState.stencilFunc,
                  depthStencilState.stencilRef, depthStencilState.stencilMask);
          glStencilOp(depthStencilState.stencilOpSfail,
                                  depthStencilState.stencilOpDPFail,
                                  depthStencilState.stencilOpDPPass);
        } else
          glDisable(GL_STENCIL_TEST);
  }

if (mask & StateGroupMask::rasterizerState) {
  glPolygonMode(GL_FRONT_AND_BACK, rasterizerState.fillMode);
  glDisable(GL_CULL_FACE);
}

if (mask & StateGroupMask::VertexBuffers) {
  for (auto &&vbo : vbos)
        glBindVertexBuffer(vbo.slot, vbo.buf.obj, vbo.buf.offset, vbo.stride);
}

if (mask & StateGroupMask::UniformBuffers) {
  for (auto &&ubo : ubos)
        glBindBufferRange(GL_UNIFORM_BUFFER, ubo.slot, ubo.buf.obj,
                                          ubo.buf.offset, ubo.buf.size);
}

if (mask & StateGroupMask::ShaderStorageBuffers) {
  for (auto &&ssbo : ssbos)
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo.slot, ssbo.buf.obj,
                                          ssbo.buf.offset, ssbo.buf.size);
}

if (mask & StateGroupMask::Images) {
  for (auto &&image : images)
        glBindImageTextures(image.slot, 1, images.tex->object());
}

if (mask & StateGroupMask::Textures) {
  for (auto &&tex : textures)
        glBindTextureUnit(tex.slot, tex.tex->object());
}

if (mask & StateGroupMask::Textures) {
  for (auto &&s : samplers)
        glBindSampler(s.slot, s.sampler->object());
}

if (mask & StateGroupMask::IndexBuffer) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.obj);
  bindContext.indexBufferOffset = indexBuffer.offset;
}

if (mask & StateGroupMask::Program)
  glUseProgram(prog->object());

if (mask & StateGroupMask::VertexArray)
  glBindVertexArray(vao->object());

if (mask & StateGroupMask::Framebuffer)
  glBindFramebuffer(fbo->object());*/
//}
}
}