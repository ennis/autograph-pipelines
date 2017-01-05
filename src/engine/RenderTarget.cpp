#include <autograph/engine/RenderTarget.h>

namespace ag {
RenderTarget::RenderTarget(
    int w, int h, std::initializer_list<ImageFormat> colorFormats,
    variant<DepthRenderbuffer, DepthTexture, NoDepth> depthFormat,
    variant<Multisample, NoMultisample> multisampling) 
{
  if (auto ms = multisampling.target<Multisample>()) {
    // TODO
    numSamples_ = ms->numSamples;
  }
  if (auto renderbuffer = depthFormat.target<DepthRenderbuffer>()) {
    depthTarget_ = gl::Renderbuffer{w, h, renderbuffer->fmt};
    fbo_.setRenderbufferAttachement(GL_DEPTH_ATTACHMENT,
                                    *depthTarget_.target<gl::Renderbuffer>());
  } else if (auto depthtex = depthFormat.target<DepthTexture>()) {
    depthTarget_ = gl::Texture::create2D(depthtex->fmt, w, h);
    fbo_.setAttachement(GL_DEPTH_ATTACHMENT,
                        *depthTarget_.target<gl::Texture>());
  }
  int index = 0;
  for (auto fmt : colorFormats) {
    colorTargets_.push_back(gl::Texture::create2D(fmt, w, h));
    fbo_.setAttachement(GL_COLOR_ATTACHMENT0 + index, colorTargets_[index]);
    ++index;
  }
}

void RenderTarget::resize(int w, int h) {
  if (auto renderbuffer = depthTarget_.target<gl::Renderbuffer>()) {
    depthTarget_ = gl::Renderbuffer{w, h, renderbuffer->format()};
    fbo_.setRenderbufferAttachement(GL_DEPTH_ATTACHMENT,
                                    *depthTarget_.target<gl::Renderbuffer>());
  } else if (auto depthtex = depthTarget_.target<gl::Texture>()) {
    depthTarget_ = gl::Texture::create2D(depthtex->format(), w, h);
    fbo_.setAttachement(GL_DEPTH_ATTACHMENT,
                        *depthTarget_.target<gl::Texture>());
  }
  for (int index = 0; index < static_cast<int>(colorTargets_.size()); index++) {
    colorTargets_[index] =
        gl::Texture::create2D(colorTargets_[index].format(), w, h);
    fbo_.setAttachement(GL_COLOR_ATTACHMENT0 + index, colorTargets_[index]);
  }
}

}
