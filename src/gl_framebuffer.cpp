#include "gl_framebuffer.hpp"


gl_framebuffer gl_framebuffer::create()
{
	GLuint obj;
	gl::CreateFramebuffers(1, &obj);
	return gl_framebuffer{ obj };
}
