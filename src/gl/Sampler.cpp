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

void Sampler::setWrapModeU(GLenum mode) {
  glSamplerParameteri(object(), GL_TEXTURE_WRAP_R, mode);
}

void Sampler::setWrapModeV(GLenum mode) {
  glSamplerParameteri(object(), GL_TEXTURE_WRAP_T, mode);
}

void Sampler::setWrapModeW(GLenum mode) {
  glSamplerParameteri(object(), GL_TEXTURE_WRAP_S, mode);
}

void Sampler::setTextureMinFilter(GLenum filter) {
  glSamplerParameteri(object(), GL_TEXTURE_MIN_FILTER, filter);
}

void Sampler::setTextureMagFilter(GLenum filter) {
  glSamplerParameteri(object(), GL_TEXTURE_MAG_FILTER, filter);
}

void Sampler::setBorderColor(float r, float g, float b, float a) {
  setBorderColor(vec4{r, g, b, a});
}

void Sampler::setBorderColor(const vec4 &rgba) {
  glSamplerParameterfv(object(), GL_TEXTURE_BORDER_COLOR, &rgba[0]);
}
}
}