#pragma once
#include "texture.hpp"

namespace ag 
{
struct framebuffer_deleter {
  void operator()(GLuint fbo) { glDeleteFramebuffers(1, &fbo); }
};

class framebuffer {
public:
  framebuffer() {}
  framebuffer(GLuint obj) : obj_{obj} {}

  static framebuffer create(std::initializer_list<texture*> color_tex);
  static framebuffer create(std::initializer_list<texture*> color_tex,
                 texture &depth_tex);
  static framebuffer create_default(glm::ivec2 size);

  void init();
  auto object() { return obj_.get(); }
  auto width() const { return width_; }
  auto height() const { return height_; }

private:
  unsigned width_;
  unsigned height_;
  gl_handle<framebuffer_deleter> obj_;
};
}
