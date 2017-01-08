#pragma once
#include "GLHandle.h"

namespace ag {
namespace gl {
struct SamplerDesc {
  GLenum addrU{ GL_REPEAT };
  GLenum addrV{ GL_REPEAT };
  GLenum addrW{ GL_REPEAT };
  GLenum minFilter{ GL_NEAREST };
  GLenum magFilter{ GL_NEAREST };
};

struct SamplerDeleter {
  void operator()(GLuint sam_obj) { glDeleteSamplers(1, &sam_obj); }
};

// Delayed initialization
class Sampler {
public:
  Sampler() {}
  Sampler(const SamplerDesc &desc) : desc_{desc} {}

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