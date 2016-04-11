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

void gl_scheduler::execute_op(op &op) {
  switch (op.opcode) {
  case gl_opcode::clear_fbo_float: {
    auto &p = static_cast<op_clear_fbo_float &>(op);
    
  } break;
  case gl_opcode::clear_tex_float: {
	  auto& p = static_cast<op_clear_tex_float &>(op);
	  gl::ClearTexImage(p.target->obj_.get(), p.level, gl::RGBA, gl::FLOAT, p.clear_color);
  } break;
  }
}

void gl_scheduler::op_clear_fbo_float::execute(gl_scheduler & sched)
{
	gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
	gl::ClearColor(clear_color[0], clear_color[1], clear_color[2],
		clear_color[2]);
	gl::Clear(bufmask);
}

void gl_scheduler::op_clear_fbo_integer::execute(gl_scheduler & sched)
{
	// TODO
}

void gl_scheduler::op_clear_tex_float::execute(gl_scheduler & sched)
{
}

void gl_scheduler::op_clear_tex_integer::execute(gl_scheduler & sched)
{
}

void gl_scheduler::op_copy_texture_host_device::execute(gl_scheduler & sched)
{
}

void gl_scheduler::op_copy_texture_device_device::execute(gl_scheduler & sched)
{
}

void gl_scheduler::op_memory_barrier::execute(gl_scheduler & sched)
{
}

void gl_scheduler::op_upload_uniform::execute(gl_scheduler & sched)
{
}
