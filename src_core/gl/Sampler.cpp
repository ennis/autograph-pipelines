#include <autograph/gl/Sampler.h>

namespace ag {

void Sampler::init() {
  gl::GLuint sampler_obj;
  gl::CreateSamplers(1, &sampler_obj);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MIN_FILTER, desc_.minFilter);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MAG_FILTER, desc_.magFilter);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_R, desc_.addrU);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_S, desc_.addrV);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_T, desc_.addrW);
  obj_ = sampler_obj;
}

void Sampler::setWrapModeU(gl::GLenum mode) {
  gl::SamplerParameteri(object(), gl::TEXTURE_WRAP_R, mode);
}

void Sampler::setWrapModeV(gl::GLenum mode) {
  gl::SamplerParameteri(object(), gl::TEXTURE_WRAP_T, mode);
}

void Sampler::setWrapModeW(gl::GLenum mode) {
  gl::SamplerParameteri(object(), gl::TEXTURE_WRAP_S, mode);
}

void Sampler::setTextureMinFilter(gl::GLenum filter) {
  gl::SamplerParameteri(object(), gl::TEXTURE_MIN_FILTER, filter);
}

void Sampler::setTextureMagFilter(gl::GLenum filter) {
  gl::SamplerParameteri(object(), gl::TEXTURE_MAG_FILTER, filter);
}

void Sampler::setBorderColor(float r, float g, float b, float a) {
  setBorderColor(vec4{r, g, b, a});
}

void Sampler::setBorderColor(const vec4 &rgba) {
  gl::SamplerParameterfv(object(), gl::TEXTURE_BORDER_COLOR, &rgba[0]);
}
} // namespace ag
