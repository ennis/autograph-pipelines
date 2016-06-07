#pragma once
#include "gl_framebuffer.hpp"
#include "image.hpp"
#include <initializer_list>

// helper classes
struct color_attachements {
  color_attachements(std::initializer_list<image> color_img)
      : images_{std::move(color_img)} {}

  std::vector<image> images_;
};

struct depth_attachement {
  depth_attachement(image depth_img) : depth_img_{std::move(depth_img)} {}
  // TODO renderbuffers?
  image depth_img_;
};

class framebuffer {
public:
  framebuffer(color_attachements color_att, depth_attachement depth_att)
      : color_att_{std::move(color_att)}, depth_att_{std::move(depth_att)} {
    // init opengl framebuffer
    gl_fbo_.init();
    int i = 0;
    for (const auto &img : color_att_.images_) {
      if (img.storage_type() != storage_type::device) {
        throw std::logic_error("TODO");
      }
      gl_fbo_.attach(gl::COLOR_ATTACHMENT0 + i, *img.texture());
    }
    gl_fbo_.attach(gl::DEPTH_ATTACHMENT, *depth_att_.depth_img_.texture());
  }

  gl_framebuffer &get_gl_framebuffer() { return gl_fbo_; }
  const std::vector<image> &get_color_attachements() {
    return color_att_.images_;
  }

  image &get_depth_attachement() { return depth_att_.depth_img_; }

private:
  color_attachements color_att_;
  depth_attachement depth_att_;
  gl_framebuffer gl_fbo_;
};