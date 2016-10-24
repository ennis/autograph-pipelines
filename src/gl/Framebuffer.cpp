#include <autograph/gl/Framebuffer.h>

namespace ag {
namespace gl {

void Framebuffer::init() {
  GLuint fbo;
  glCreateFramebuffers(1, &fbo);
  obj_ = fbo;
}

void bindFramebufferTextures(GLuint fbo,
                             std::initializer_list<Texture *> color_tex,
                             unsigned &width, unsigned &height) {
  int index = 0;
  width = 0;
  height = 0;
  for (auto tex : color_tex) {
    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + index, tex->object(),
                              0);
    ++index;
    if (((width != 0) && (width != tex->width())) ||
        ((height != 0) && (height != tex->height())))
      throw std::logic_error(
          "The dimensions of the framebuffer attachements do not match");
    else {
      width = tex->width();
      height = tex->height();
    }
  }

  static const GLenum drawBuffers[8] = {
      GL_COLOR_ATTACHMENT0,     GL_COLOR_ATTACHMENT0 + 1,
      GL_COLOR_ATTACHMENT0 + 2, GL_COLOR_ATTACHMENT0 + 3,
      GL_COLOR_ATTACHMENT0 + 4, GL_COLOR_ATTACHMENT0 + 5,
      GL_COLOR_ATTACHMENT0 + 6, GL_COLOR_ATTACHMENT0 + 7};
  glNamedFramebufferDrawBuffers(fbo, static_cast<GLsizei>(color_tex.size()),
                                drawBuffers);
}

Framebuffer Framebuffer::create(std::initializer_list<Texture *> color_tex) {
  Framebuffer out;
  out.init();
  bindFramebufferTextures(out.object(), color_tex, out.width_, out.height_);
  return out;
}

Framebuffer Framebuffer::create(std::initializer_list<Texture *> color_tex,
                                Texture &depth_tex) {
  Framebuffer out;
  out.init();
  bindFramebufferTextures(out.object(), color_tex, out.width_, out.height_);
  glNamedFramebufferTexture(out.object(), GL_DEPTH_ATTACHMENT,
                            depth_tex.object(), 0);
  return out;
}

Framebuffer Framebuffer::createDefault(glm::ivec2 size) {
  Framebuffer out;
  out.width_ = static_cast<unsigned>(size.x);
  out.height_ = static_cast<unsigned>(size.y);
  return out;
}
}
}