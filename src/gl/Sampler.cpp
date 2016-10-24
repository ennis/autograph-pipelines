#include <autograph/gl/Sampler.h>

namespace ag {
namespace gl {

void Sampler::init() {
  GLuint sampler_obj;
  glCreateSamplers(1, &sampler_obj);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_MIN_FILTER, desc_.min_filter);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_MAG_FILTER, desc_.mag_filter);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_WRAP_R, desc_.tex_wrap_r);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_WRAP_S, desc_.tex_wrap_s);
  glSamplerParameteri(sampler_obj, GL_TEXTURE_WRAP_T, desc_.tex_wrap_t);
  obj_ = sampler_obj;
}
}
}