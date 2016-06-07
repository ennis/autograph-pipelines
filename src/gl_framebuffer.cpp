#include "gl_framebuffer.hpp"

void gl_framebuffer::init() {
  GLuint fbo;
  gl::CreateFramebuffers(1, &fbo);
  obj_ = fbo;
}

void gl_framebuffer::attach(GLenum attachement, gl_texture & tex)
{
	gl::NamedFramebufferTexture(obj_.get(), attachement, tex.obj_.get(), 0);
	if (size_.x != 0) 
		size_ = tex.size();
	else 
		if (size_ != tex.size())
			throw std::logic_error("Invalid framebuffer attachements");
}

void gl_framebuffer::set_draw_buffers(int num_buffers)
{
	static const GLenum drawBuffers[8] = {
		gl::COLOR_ATTACHMENT0,     gl::COLOR_ATTACHMENT0 + 1,
		gl::COLOR_ATTACHMENT0 + 2, gl::COLOR_ATTACHMENT0 + 3,
		gl::COLOR_ATTACHMENT0 + 4, gl::COLOR_ATTACHMENT0 + 5,
		gl::COLOR_ATTACHMENT0 + 6, gl::COLOR_ATTACHMENT0 + 7 };
	gl::NamedFramebufferDrawBuffers(obj_.get(), (GLsizei)num_buffers, drawBuffers);
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

}

gl_framebuffer::gl_framebuffer(gsl::span<gl_texture*> color_attachements) {
  init();
  int i = 0;
  for (auto tex : color_attachements) {
	  attach(gl::COLOR_ATTACHMENT0 + i, *tex);
	  ++i;
  }
}

gl_framebuffer::gl_framebuffer(gsl::span<gl_texture*> color_attachements,
                               gl_texture &depth_tex) {
  init();
  int i = 0;
  for (auto tex : color_attachements) {
	  attach(gl::COLOR_ATTACHMENT0 + i, *tex);
	  ++i;
  }
  attach(gl::DEPTH_ATTACHMENT, depth_tex);
}
