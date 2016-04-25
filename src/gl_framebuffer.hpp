#pragma once
#include "gl_handle.hpp"
#include "gl_texture.hpp"
#include <gl_core_4_5.hpp>
#include <gsl.h>

struct framebuffer_deleter {
  void operator()(GLuint fbo) { gl::DeleteFramebuffers(1, &fbo); }
};

struct gl_framebuffer {
  gl_framebuffer() {}
  gl_framebuffer(GLuint obj) : obj_{obj} {}
  gl_framebuffer(gsl::span<gl_texture*> color_attachements);
  gl_framebuffer(gsl::span<gl_texture*> color_attachements,
                 gl_texture &depth_attachement);

  void init();

  unsigned width;
  unsigned height;

  gl_handle<framebuffer_deleter> obj_;
};
