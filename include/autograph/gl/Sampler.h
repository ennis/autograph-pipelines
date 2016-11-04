#pragma once
#include "GLHandle.h"

namespace ag {
namespace gl {
struct SamplerDesc {
  GLenum addrU;
  GLenum addrV;
  GLenum addrW;
  GLenum minFilter;
  GLenum magFilter;
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