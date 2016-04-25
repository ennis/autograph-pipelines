#include "gl_sampler.hpp"

void gl_sampler::init() {
  GLuint sampler_obj;
  gl::CreateSamplers(1, &sampler_obj);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MIN_FILTER, desc_.min_filter);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MAG_FILTER, desc_.mag_filter);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_R, desc_.tex_wrap_r);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_S, desc_.tex_wrap_s);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_T, desc_.tex_wrap_t);
  obj_ = sampler_obj;
}

