#pragma once
#include <autograph/Config.h>
#include "GLHandle.h"

namespace ag {
namespace gl {
struct AG_GL_API SamplerDesc {
  GLenum addrU{ GL_REPEAT };
  GLenum addrV{ GL_REPEAT };
  GLenum addrW{ GL_REPEAT };
  GLenum minFilter{ GL_NEAREST };
  GLenum magFilter{ GL_NEAREST };
};

struct AG_GL_API SamplerDeleter {
	static constexpr GLenum objectType = GL_SAMPLER;
  void operator()(GLuint sam_obj) { glDeleteSamplers(1, &sam_obj); }
};

// Delayed initialization
class AG_GL_API Sampler {
public:
  Sampler() {}
  Sampler(const SamplerDesc &desc) : desc_{desc} {}

  GLuint object() {
    if (!obj_)
      init();
    return obj_.get();
  }

  void setWrapModeU(GLenum mode);
  void setWrapModeV(GLenum mode);
  void setWrapModeW(GLenum mode);
  void setTextureMinFilter(GLenum filter);
  void setTextureMagFilter(GLenum filter);
  void setBorderColor(float r, float g, float b, float a);
  void setBorderColor(const vec4& rgba);

private:
  SamplerDesc desc_;
  GLHandle<SamplerDeleter> obj_;
  void init();
};
}
}