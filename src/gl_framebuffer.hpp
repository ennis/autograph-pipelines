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
  void attach(GLenum attachement, gl_texture& tex);
  void set_draw_buffers(int num_buffers);
  glm::ivec2 size() const { return size_; }
  GLuint object() const { return obj_.get(); }

  glm::ivec2 size_;
  gl_handle<framebuffer_deleter> obj_;
};
