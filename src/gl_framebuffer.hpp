#pragma once
#include "gl_handle.hpp"
#include "gl_texture.hpp"
#include <gl_core_4_5.hpp>

struct framebuffer_deleter
{
	void operator()(GLuint fbo) { gl::DeleteFramebuffers(1, &fbo); }
};

struct gl_framebuffer
{
	gl_framebuffer(GLuint obj) : obj_{ obj }
	{}

	gl_handle<framebuffer_deleter> obj_;
	//void attach(GLenum attachement, gl_texture & tex, unsigned mip_level = 0, unsigned layer = 0);
	//GLenum check_status(GLenum target = gl::DRAW_FRAMEBUFFER);

	static gl_framebuffer create();
};