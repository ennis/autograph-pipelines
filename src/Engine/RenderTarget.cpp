#include <autograph/Engine/RenderTarget.h>
#include <autograph/Core/Support/Debug.h>

namespace ag {
RenderTarget::RenderTarget(
    int w, int h, std::initializer_list<ImageFormat> colorFormats,
    variant<DepthRenderbuffer, DepthTexture, NoDepth> depthFormat,
	Samples samples)
{
    numSamples_ = samples.count;
  if (auto renderbuffer = depthFormat.target<DepthRenderbuffer>()) {
	  depthTarget_ = Renderbuffer{ w, h, renderbuffer->fmt, Renderbuffer::Samples{ samples.count } };
    fbo_.setRenderbufferAttachement(gl::DEPTH_ATTACHMENT,
                                    *depthTarget_.target<Renderbuffer>());
  } else if (auto depthtex = depthFormat.target<DepthTexture>()) {
	  depthTarget_ = Texture::create2DMultisample(depthtex->fmt, w, h, Texture::Samples{ samples.count });
    fbo_.setAttachement(gl::DEPTH_ATTACHMENT,
                        *depthTarget_.target<Texture>());
  }
  int index = 0;
  for (auto fmt : colorFormats) {
    colorTargets_.push_back(Texture::create2DMultisample(fmt, w, h, Texture::Samples{ samples.count }));
    fbo_.setAttachement(gl::COLOR_ATTACHMENT0 + index, colorTargets_[index]);
    ++index;
  }
}

void RenderTarget::resize(int w, int h) {
	using namespace gl;
  if (auto renderbuffer = depthTarget_.target<Renderbuffer>()) {
    depthTarget_ = Renderbuffer{w, h, renderbuffer->format(), Renderbuffer::Samples{ numSamples_ } };
    fbo_.setRenderbufferAttachement(gl::DEPTH_ATTACHMENT,
                                    *depthTarget_.target<Renderbuffer>());
  } else if (auto depthtex = depthTarget_.target<Texture>()) {
	  depthTarget_ = Texture::create2DMultisample(depthtex->format(), w, h, Texture::Samples{ numSamples_ });
    fbo_.setAttachement(gl::DEPTH_ATTACHMENT,
                        *depthTarget_.target<Texture>());
  }
  for (int index = 0; index < static_cast<int>(colorTargets_.size()); index++) {
    colorTargets_[index] =
		Texture::create2DMultisample(colorTargets_[index].format(), w, h, Texture::Samples{ numSamples_ });
    fbo_.setAttachement(gl::COLOR_ATTACHMENT0 + index, colorTargets_[index]);
  }
}

}
