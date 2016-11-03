#pragma once
#include <autograph/Types.h>
#include <autograph/Config.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/GLHandle.h>

namespace ag {
namespace gl {
//////////////////////////////////////////////
struct FramebufferDeleter {
  void operator()(GLuint fbo) { glDeleteFramebuffers(1, &fbo); }
};

//////////////////////////////////////////////
/**
 * @brief Framebuffer
 * @details Framebuffer
 */
class AG_API Framebuffer {
public:
  Framebuffer() {}
  Framebuffer(GLHandle<FramebufferDeleter> obj) : obj_{std::move(obj)} {}

  static Framebuffer create(std::initializer_list<Texture *> color_tex);
  static Framebuffer create(std::initializer_list<Texture *> color_tex,
                            Texture &depth_tex);
  static Framebuffer createDefault(ivec2 size);

  void init();
  auto object() const { return obj_.get(); }
  auto width() const { return width_; }
  auto height() const { return height_; }

private:
  unsigned width_;
  unsigned height_;
  GLHandle<FramebufferDeleter> obj_;
};
}
}