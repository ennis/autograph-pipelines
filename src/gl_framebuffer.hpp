#pragma once
#include "gl_handle.hpp"
#include "gl_texture.hpp"
#include <gl_core_4_5.hpp>

struct framebuffer_deleter {
  void operator()(GLuint fbo) { gl::DeleteFramebuffers(1, &fbo); }
};

struct gl_framebuffer {
  gl_framebuffer() {}
  gl_framebuffer(GLuint obj) : obj_{obj} {}
  gl_framebuffer(std::initializer_list<gl_texture &> color_tex);
  gl_framebuffer(std::initializer_list<gl_texture &> color_tex,
                 gl_texture &depth_tex);

  void init();

  unsigned width;
  unsigned height;

  gl_handle<framebuffer_deleter> obj_;
};
