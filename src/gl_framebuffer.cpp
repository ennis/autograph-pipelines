#include "gl_framebuffer.hpp"

void gl_framebuffer::init() {
  GLuint fbo;
  gl::CreateFramebuffers(1, &fbo);
  obj_ = fbo;
}

void bind_framebuffer_textures(GLuint fbo,
								gsl::span<gl_texture*> color_tex,
                               unsigned &width, unsigned &height) {
  int index = 0;
  width = 0;
  height = 0;
  for (auto &tex : color_tex) {
    gl::NamedFramebufferTexture(fbo, gl::COLOR_ATTACHMENT0 + index,
                                tex->obj_.get(), 0);
    ++index;
	if (width != 0) {
		width = tex->desc_.width;
		height = tex->desc_.height;
	}else {
		if (width != tex->desc_.width || height != tex->desc_.height)
			throw std::logic_error("Invalid framebuffer attachements");		
	}
  }

  static const GLenum drawBuffers[8] = {
      gl::COLOR_ATTACHMENT0,     gl::COLOR_ATTACHMENT0 + 1,
      gl::COLOR_ATTACHMENT0 + 2, gl::COLOR_ATTACHMENT0 + 3,
      gl::COLOR_ATTACHMENT0 + 4, gl::COLOR_ATTACHMENT0 + 5,
      gl::COLOR_ATTACHMENT0 + 6, gl::COLOR_ATTACHMENT0 + 7};
  gl::NamedFramebufferDrawBuffers(fbo, (GLsizei)color_tex.size(), drawBuffers);
}

gl_framebuffer::gl_framebuffer(gsl::span<gl_texture*> color_attachements) {
  init();
  bind_framebuffer_textures(obj_.get(), color_attachements, width, height);
}

gl_framebuffer::gl_framebuffer(gsl::span<gl_texture*> color_attachements,
                               gl_texture &depth_tex) {
  init();
  bind_framebuffer_textures(obj_.get(), color_attachements, width, height);
  gl::NamedFramebufferTexture(obj_.get(), gl::DEPTH_ATTACHMENT,
                              depth_tex.obj_.get(), 0);
}
