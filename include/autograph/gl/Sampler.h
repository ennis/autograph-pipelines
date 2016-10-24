#pragma once
#include "GLHandle.h"

namespace ag {
namespace gl {
struct SamplerDesc {
  GLenum tex_wrap_s;
  GLenum tex_wrap_t;
  GLenum tex_wrap_r;
  GLenum min_filter;
  GLenum mag_filter;
};

struct SamplerDeleter {
  void operator()(GLuint sam_obj) { glDeleteSamplers(1, &sam_obj); }
};

// Delayed initialization
class Sampler {
public:
  Sampler() {}
  Sampler(const SamplerDesc &desc) : desc_{desc} {}
  ~Sampler() {}

  GLuint object() {
    if (!obj_)
      init();
    return obj_.get();
  }

private:
  SamplerDesc desc_;
  GLHandle<SamplerDeleter> obj_;
  void init();
};
}
}