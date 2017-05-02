#pragma once
#include <autograph/Config.h>
#include "GLHandle.h"

namespace ag {

struct AG_GFX_API SamplerDesc {
  gl::GLenum addrU{ gl::REPEAT };
  gl::GLenum addrV{ gl::REPEAT };
  gl::GLenum addrW{ gl::REPEAT };
  gl::GLenum minFilter{ gl::NEAREST };
  gl::GLenum magFilter{ gl::NEAREST };
};

struct AG_GFX_API SamplerDeleter {
	static constexpr gl::GLenum objectType = gl::SAMPLER;
  void operator()(gl::GLuint sam_obj) { gl::DeleteSamplers(1, &sam_obj); }
};

// Delayed initialization
class AG_GFX_API Sampler {
public:
  Sampler() {}
  Sampler(const SamplerDesc &desc) : desc_{desc} {}

  gl::GLuint object() {
    if (!obj_)
      init();
    return obj_.get();
  }

  void setWrapModeU(gl::GLenum mode);
  void setWrapModeV(gl::GLenum mode);
  void setWrapModeW(gl::GLenum mode);
  void setTextureMinFilter(gl::GLenum filter);
  void setTextureMagFilter(gl::GLenum filter);
  void setBorderColor(float r, float g, float b, float a);
  void setBorderColor(const vec4& rgba);

private:
  SamplerDesc desc_;
  GLHandle<SamplerDeleter> obj_;
  void init();
};
}
