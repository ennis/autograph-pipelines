#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Texture.h>
#include <stdexcept>

namespace ag {

Renderbuffer::Renderbuffer(int w, int h, ImageFormat fmt, Samples samples)
    : width_{w}, height_{h}, fmt_{fmt} {
  auto &fmtinfo = getGLImageFormatInfo(fmt);
  gl::GLuint rbuf;
  gl::CreateRenderbuffers(1, &rbuf);
  if (samples.count)
	  gl::NamedRenderbufferStorageMultisample(rbuf, samples.count, fmtinfo.internal_fmt, w, h);
  else
  gl::NamedRenderbufferStorage(rbuf, fmtinfo.internal_fmt, w, h);
  obj_ = rbuf;
}

void Framebuffer::initialize() {}

Framebuffer::Framebuffer(std::initializer_list<Texture *> colorTargets_,
                         Texture *depthTarget_) {
  int index = 0;
  for (auto tex : colorTargets_) {
    if (tex)
      setAttachement(gl::COLOR_ATTACHMENT0 + index, *tex);
    ++index;
  }
  if (depthTarget_) {
    setAttachement(gl::DEPTH_ATTACHMENT, *depthTarget_);
  }
}

void Framebuffer::setAttachement(gl::GLenum attachement, gl::GLuint tex) {
  ensureInitialized();
  gl::NamedFramebufferTexture(obj_.get(), attachement, tex, 0);
}

void Framebuffer::setAttachement(gl::GLenum attachement, Texture &tex) {
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

void Framebuffer::setRenderbufferAttachement(gl::GLenum attachement,
                                             Renderbuffer &renderbuffer) {
  checkDimensions(renderbuffer.width(), renderbuffer.height());
  ensureInitialized();
  gl::NamedFramebufferRenderbuffer(obj_.get(), attachement, gl::RENDERBUFFER,
                                 renderbuffer.object());
}

void Framebuffer::ensureInitialized() {
  if (!obj_) {
	  gl::GLuint fbo;
    gl::CreateFramebuffers(1, &fbo);
    obj_ = fbo;
    static const gl::GLenum drawBuffers[8] = {
        gl::COLOR_ATTACHMENT0,     gl::COLOR_ATTACHMENT0 + 1,
        gl::COLOR_ATTACHMENT0 + 2, gl::COLOR_ATTACHMENT0 + 3,
        gl::COLOR_ATTACHMENT0 + 4, gl::COLOR_ATTACHMENT0 + 5,
        gl::COLOR_ATTACHMENT0 + 6, gl::COLOR_ATTACHMENT0 + 7};
    // do this once
    gl::NamedFramebufferDrawBuffers(fbo, 8, drawBuffers);
  }
}

Framebuffer Framebuffer::createDefault(int w, int h) {
  Framebuffer out;
  out.width_ = w;
  out.height_ = h;
  return out;
}

bool Framebuffer::ensureComplete() {
  return checkStatus() == gl::FRAMEBUFFER_COMPLETE;
}

gl::GLenum Framebuffer::checkStatus() {
  return gl::CheckNamedFramebufferStatus(obj_.get(), gl::DRAW_FRAMEBUFFER);
}

}