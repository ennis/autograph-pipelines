#pragma once
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Texture.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Variant.h>

namespace ag {
class AG_API RenderTarget {
public:
	struct DepthRenderbuffer { ImageFormat fmt; };
	struct DepthTexture { ImageFormat fmt; };
	struct NoDepth {}; 
  struct Multisample { int numSamples; };
  struct NoMultisample {};

  RenderTarget(int w, int h, std::initializer_list<ImageFormat> colorFormats,
               variant<DepthRenderbuffer, DepthTexture, NoDepth> depthFormat,
               variant<Multisample, NoMultisample> multisampling = NoMultisample{});

  void resize(int w, int h);

  int getColorTargetCount() { return static_cast<int>(colorTargets_.size()); }
  auto getColorTarget(int index) -> gl::Texture & {
    return colorTargets_[index];
  }
  auto getDepthTarget() -> variant<gl::Texture,gl::Renderbuffer>& { return depthTarget_; }
  auto getFramebuffer() -> gl::Framebuffer & { return fbo_; }
  auto getSampleCount() { return numSamples_; }

  // addColorTarget
  // addColorTargetMultisample
  // addDepthTarget
  // addDepthRenderbuffer
  // 

private:
  SmallVector<gl::Texture,8> colorTargets_;
  int numSamples_;
  variant<gl::Texture,gl::Renderbuffer> depthTarget_;
  gl::Framebuffer fbo_;
};

// RenderTarget rt { {ColorTarget{RGBA32F, , RGBA16F}, RenderTarget::DepthRenderbuffer{}, RenderTarget::Multisample{ 8 } };
}
