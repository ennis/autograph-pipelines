#include <autograph/gl/StateGroup.h>
#include <gl_core_4_5.h>

namespace ag {
namespace gl {
// Bind state group to GL pipeline
void StateGroup::operator()(GLBindContext &bindContext) {
  if (mask & StateGroupMask::Viewports) {
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
                               bs.state.funcDstRGB, bs.state.funcSrcAlpha,
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
    glBindFramebuffer(fbo->object());
}
}
}