#pragma once
#include <autograph/engine/Config.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Texture.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Variant.h>

namespace ag {
class AG_ENGINE_API RenderTarget {
public:
  struct DepthRenderbuffer {
    ImageFormat fmt;
  };
  struct DepthTexture {
    ImageFormat fmt;
  };
  struct NoDepth {};
  struct Samples {
    int count;
  };

  RenderTarget() = default;

  RenderTarget(int w, int h, std::initializer_list<ImageFormat> colorFormats,
               variant<DepthRenderbuffer, DepthTexture, NoDepth> depthFormat,
               Samples samples = Samples{0});

  void resize(int w, int h);

  int getColorTargetCount() { return static_cast<int>(colorTargets_.size()); }
  auto getColorTarget(int index) -> Texture & { return colorTargets_[index]; }
  auto getDepthTarget() -> variant<Texture, Renderbuffer> & {
    return depthTarget_;
  }
  auto getFramebuffer() -> Framebuffer & { return fbo_; }
  auto getSampleCount() { return numSamples_; }

  // addColorTarget
  // addColorTargetMultisample
  // addDepthTarget
  // addDepthRenderbuffer
  //

private:
  SmallVector<Texture, 8> colorTargets_;
  int numSamples_{0};
  variant<Texture, Renderbuffer> depthTarget_;
  Framebuffer fbo_;
};

} // namespace ag
