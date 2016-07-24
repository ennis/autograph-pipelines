#pragma once
#include "gl_handle.hpp"

namespace ag {
struct sampler_desc {
  GLenum tex_wrap_s;
  GLenum tex_wrap_t;
  GLenum tex_wrap_r;
  GLenum min_filter;
  GLenum mag_filter;
};

struct sampler_deleter {
  void operator()(GLuint sam_obj) { glDeleteSamplers(1, &sam_obj); }
};

// Delayed initialization
class sampler {
public:
  sampler() {}
  sampler(const sampler_desc &desc) : desc_{desc} {}
  ~sampler() {}

  GLuint object() {
    if (!obj_)
      init();
    return obj_.get();
  }

private:
  sampler_desc desc_;
  gl_handle<sampler_deleter> obj_;
  void init();
};

}