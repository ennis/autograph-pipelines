#include <autograph/gl/Sampler.h>

namespace ag {
namespace gl {

void Sampler::init() {
  GLuint sampler_obj;
  glCreateSamplers(1, &sampler_obj);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_MIN_FILTER, desc_.minFilter);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_MAG_FILTER, desc_.magFilter);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_WRAP_R, desc_.addrU);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_WRAP_S, desc_.addrV);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_WRAP_T, desc_.addrW);
  obj_ = sampler_obj;
}
}
}