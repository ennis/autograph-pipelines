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

  RenderTarget(int w, int h, std::initializer_list<ImageFormat> colorFormats,
               variant<DepthRenderbuffer, DepthTexture, NoDepth> depthFormat);

  void resize(int w, int h);

  int getColorTargetCount() { return static_cast<int>(colorTargets_.size()); }
  auto getColorTarget(int index) -> gl::Texture & {
    return colorTargets_[index];
  }
  auto getDepthTarget() -> variant<gl::Texture,gl::Renderbuffer>& { return depthTarget_; }
  auto getFramebuffer() -> gl::Framebuffer & { return fbo_; }

private:
  SmallVector<gl::Texture,8> colorTargets_;
  variant<gl::Texture,gl::Renderbuffer> depthTarget_;
  gl::Framebuffer fbo_;
};

// RenderTarget rt { {RGBA32F, RGBA16F}, RenderTarget::DepthRenderbuffer{} };
}