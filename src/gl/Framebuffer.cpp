#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Texture.h>
#include <stdexcept>

namespace ag {
namespace gl {

Renderbuffer::Renderbuffer(int w, int h, ImageFormat fmt, Samples samples)
    : width_{w}, height_{h}, fmt_{fmt} {
  auto &fmtinfo = getGLImageFormatInfo(fmt);
  GLuint rbuf;
  glCreateRenderbuffers(1, &rbuf);
  if (samples.count)
	  glNamedRenderbufferStorageMultisample(rbuf, samples.count, fmtinfo.internal_fmt, w, h);
  else
  glNamedRenderbufferStorage(rbuf, fmtinfo.internal_fmt, w, h);
  obj_ = rbuf;
}

void Framebuffer::initialize() {}

Framebuffer::Framebuffer(std::initializer_list<Texture *> colorTargets_,
                         Texture *depthTarget_) {
  int index = 0;
  for (auto tex : colorTargets_) {
    if (tex)
      setAttachement(GL_COLOR_ATTACHMENT0 + index, *tex);
    ++index;
  }
  if (depthTarget_) {
    setAttachement(GL_DEPTH_ATTACHMENT, *depthTarget_);
  }
}

void Framebuffer::setAttachement(GLenum attachement, GLuint tex) {
  ensureInitialized();
  glNamedFramebufferTexture(obj_.get(), attachement, tex, 0);
}

void Framebuffer::setAttachement(GLenum attachement, Texture &tex) {
  checkDimensions(tex.width(), tex.height());
  setAttachement(attachement, tex.object());
}

void Framebuffer::checkDimensions(int w, int h) {
  if (((width_ != 0) && (width_ != w)) || ((height_ != 0) && (height_ != h)))
    throw std::logic_error(
        "The dimensions of the framebuffer attachements do not match");
  else {
    width_ = w;
    height_ = h;
  }
}

void Framebuffer::setRenderbufferAttachement(GLenum attachement,
                                             Renderbuffer &renderbuffer) {
  checkDimensions(renderbuffer.width(), renderbuffer.height());
  ensureInitialized();
  glNamedFramebufferRenderbuffer(obj_.get(), attachement, GL_RENDERBUFFER,
                                 renderbuffer.object());
}

void Framebuffer::ensureInitialized() {
  if (!obj_) {
    GLuint fbo;
    glCreateFramebuffers(1, &fbo);
    obj_ = fbo;
    static const GLenum drawBuffers[8] = {
        GL_COLOR_ATTACHMENT0,     GL_COLOR_ATTACHMENT0 + 1,
        GL_COLOR_ATTACHMENT0 + 2, GL_COLOR_ATTACHMENT0 + 3,
        GL_COLOR_ATTACHMENT0 + 4, GL_COLOR_ATTACHMENT0 + 5,
        GL_COLOR_ATTACHMENT0 + 6, GL_COLOR_ATTACHMENT0 + 7};
    // do this once
    glNamedFramebufferDrawBuffers(fbo, 8, drawBuffers);
  }
}

Framebuffer Framebuffer::createDefault(int w, int h) {
  Framebuffer out;
  out.width_ = w;
  out.height_ = h;
  return out;
}

bool Framebuffer::ensureComplete() {
  return checkStatus() == GL_FRAMEBUFFER_COMPLETE;
}

GLenum Framebuffer::checkStatus() {
  return glCheckNamedFramebufferStatus(obj_.get(), GL_DRAW_FRAMEBUFFER);
}
}
}