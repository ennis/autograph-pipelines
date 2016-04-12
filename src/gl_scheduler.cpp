#include "gl_scheduler.hpp"
#include "image.hpp"

namespace {
void gl_bind_render_target(GLuint fbo, rect_2d viewport) {
  gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo);
  // gl::BindViewport
}
}

void gl_scheduler::optimize(std::shared_ptr<value_impl> v) {}

void gl_scheduler::evaluate(const image_impl &img, const gl_surface &target) {
  // check if img is marked dirty
  // yes: eval img
  // no: return resource associated w/ img

  // eval img:
  //	eval dependencies
  //    execute node
  //    unmark dirty

  // if there is no plan for img
  //   create plan
  //   allocate output resources
}

void gl_scheduler::schedule(const value_impl &img) {}

void gl_scheduler::execute_op(op &op) {}

void gl_scheduler::op_clear_fbo_float::execute(gl_scheduler &sched) {
  gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
  gl::ClearColor(clear_color[0], clear_color[1], clear_color[2],
                 clear_color[3]);
  gl::ClearDepthf(depth);
  gl::Clear(bufmask);
}

void gl_scheduler::op_clear_fbo_integer::execute(gl_scheduler &sched) {
  // gl::ClearNamedFramebufferiv()
}

void gl_scheduler::op_clear_tex_float::execute(gl_scheduler &sched) {
  gl::ClearTexImage(target->obj_.get(), level, gl::RGBA, gl::FLOAT,
                    clear_color);
}

void gl_scheduler::op_clear_tex_integer::execute(gl_scheduler &sched) 
{
	// TODO
}

void gl_scheduler::op_copy_texture_host_device::execute(gl_scheduler &sched) {
  auto glfmt = get_gl_image_format_info(dest->desc_.format);
  // assume 2D texture
  gl::TextureSubImage2D(dest->obj_.get(), level, 0, 0, dest->desc_.width,
                        dest->desc_.height, glfmt.external_fmt, glfmt.type,
                        src);
}

void gl_scheduler::op_copy_texture_device_device::execute(gl_scheduler &sched) {
  gl::CopyImageSubData(src->obj_.get(), gl::TEXTURE_2D, srclevel, 0, 0, 0,
                       dest->obj_.get(), gl::TEXTURE_2D, dstlevel, 0, 0, 0,
                       src->desc_.width, src->desc_.height, src->desc_.depth);
}

void gl_scheduler::op_memory_barrier::execute(gl_scheduler &sched) 
{
	gl::MemoryBarrier(barriers);
}

void gl_scheduler::op_upload_uniform::execute(gl_scheduler &sched) 
{
	// ???
}
