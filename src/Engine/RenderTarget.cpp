#include <autograph/Core/Support/Debug.h>
#include <autograph/Engine/RenderTarget.h>

namespace ag {
RenderTarget::RenderTarget(
    int w, int h, std::initializer_list<ImageFormat> colorFormats,
    variant<DepthRenderbuffer, DepthTexture, NoDepth> depthFormat,
    Samples samples) {
  numSamples_ = samples.count;
  if (auto renderbuffer = ag::get_if<DepthRenderbuffer>(&depthFormat)) {
    depthTarget_ = Renderbuffer{w, h, renderbuffer->fmt,
                                Renderbuffer::Samples{samples.count}};
    fbo_.setRenderbufferAttachement(gl::DEPTH_ATTACHMENT,
                                    ag::get<Renderbuffer>(depthTarget_));
  } else if (auto depthtex = ag::get_if<DepthTexture>(&depthFormat)) {
    depthTarget_ = Texture::create2D(depthtex->fmt, w, h, Texture::MipMaps{1},
                                     Texture::Samples{samples.count});
    fbo_.setAttachement(gl::DEPTH_ATTACHMENT, ag::get<Texture>(depthTarget_));
  }
  int index = 0;
  for (auto fmt : colorFormats) {
    colorTargets_.push_back(Texture::create2D(fmt, w, h, Texture::MipMaps{1},
                                              Texture::Samples{samples.count}));
    fbo_.setAttachement(gl::COLOR_ATTACHMENT0 + index, colorTargets_[index]);
    ++index;
  }
}

void RenderTarget::resize(int w, int h) {
  using namespace gl;
  if (auto renderbuffer = ag::get_if<Renderbuffer>(&depthTarget_)) {
    depthTarget_ = Renderbuffer{w, h, renderbuffer->format(),
                                Renderbuffer::Samples{numSamples_}};
    fbo_.setRenderbufferAttachement(gl::DEPTH_ATTACHMENT,
                                    ag::get<Renderbuffer>(depthTarget_));
  } else if (auto depthtex = ag::get_if<Texture>(&depthTarget_)) {
    depthTarget_ =
        Texture::create2D(depthtex->format(), w, h, Texture::MipMaps{1},
                          Texture::Samples{numSamples_});
    fbo_.setAttachement(gl::DEPTH_ATTACHMENT, ag::get<Texture>(depthTarget_));
  }
  for (int index = 0; index < static_cast<int>(colorTargets_.size()); index++) {
    colorTargets_[index] =
        Texture::create2D(colorTargets_[index].format(), w, h,
                          Texture::MipMaps{1}, Texture::Samples{numSamples_});
    fbo_.setAttachement(gl::COLOR_ATTACHMENT0 + index, colorTargets_[index]);
  }
}

} // namespace ag
