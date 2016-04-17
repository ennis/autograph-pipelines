#include "dyn_cast.hpp"
#include "gl_limits.hpp"
#include "gl_scheduler.hpp"
#include "image.hpp"

#include <cppformat/format.h>
#include <iostream>

namespace {
void gl_bind_render_target(GLuint fbo, rect_2d viewport) {
  gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo);
  // gl::BindViewport
}

void gl_bind_draw_state(const gl_draw_state &state) {
  if (not_empty(state.mask & gl_draw_state_mask::viewports)) {
    gl::ViewportArrayv(0, (GLsizei)state.viewports.size(),
                       (const float *)(state.viewports.data()));
  }
  if (not_empty(state.mask & gl_draw_state_mask::blend_states)) {
    if (state.blend_states.empty())
      gl::Disable(gl::BLEND);
    else {
      gl::Enable(gl::BLEND); // XXX is this necessary
      int index = 0;
      for (const auto &bs : state.blend_states) {
        if (bs.enabled) {
          gl::Enablei(gl::BLEND, index);
          gl::BlendEquationSeparatei(index, bs.modeRGB, bs.modeAlpha);
          gl::BlendFuncSeparatei(index, bs.funcSrcRGB, bs.funcDstRGB,
                                 bs.funcSrcAlpha, bs.funcDstAlpha);
        } else
          gl::Disablei(gl::BLEND, index);
        index++;
      }
    }
  }
  if (not_empty(state.mask & gl_draw_state_mask::depth_stencil_state)) {
    if (state.depth_stencil_state.depthTestEnable)
      gl::Enable(gl::DEPTH_TEST);
    else
      gl::Disable(gl::DEPTH_TEST);

    if (state.depth_stencil_state.stencilEnable) {
      gl::Enable(gl::STENCIL_TEST);
      gl::StencilFuncSeparate(state.depth_stencil_state.stencilFace,
                              state.depth_stencil_state.stencilFunc,
                              state.depth_stencil_state.stencilRef,
                              state.depth_stencil_state.stencilMask);
      gl::StencilOp(state.depth_stencil_state.stencilOpSfail,
                    state.depth_stencil_state.stencilOpDPFail,
                    state.depth_stencil_state.stencilOpDPPass);
    } else
      gl::Disable(gl::STENCIL_TEST);
  }
  if (not_empty(state.mask & gl_draw_state_mask::rasterizer_state)) {
    gl::PolygonMode(gl::FRONT_AND_BACK, state.rasterizer_state.fillMode);
    gl::Disable(gl::CULL_FACE);
  }
  if (not_empty(state.mask & gl_draw_state_mask::draw_buffers)) {
    // TODO
  }
}
}

// scheduler data for buffer value nodes
struct buffer_impl_data {
  storage_type stype;
  union {
    gl_buffer_slice buf; // only if stype == device
    void *data;          // only if stype == host
  } u;
};

// scheduler data for image value nodes
struct image_impl_data {
  storage_type stype;
  union {
    GLuint tex; // only if stype == device
    void *data; // only if stype == host
  } u;
};

void gl_scheduler::optimize(std::shared_ptr<value_impl> v) {}

void gl_scheduler::evaluate_private(value_impl& v, std::vector<value_impl*>& pred_stack)
{
	auto n = v.pred_;

	if (auto nn = dyn_cast<compute_node>(n)) {
		for (auto& r : nn->res) {
			if (not_empty(r.access & shader_resource_access::read)) {
				if (auto img = dyn_cast<image_impl>(r.resource.get())) {
					evaluate_private(*img, pred_stack);
				}
			}
		}

		// dispatch compute

	}

  // eval img:
  //	eval dependencies
  //    execute node
  //    unmark dirty

  // if there is no plan for img
  //   create plan
  //   allocate output resources
}

void gl_scheduler::schedule(value_impl &img) {
  struct mark_dirty : public traversal_visitor {
    mark_dirty(gl_scheduler &sched) : sched_{sched} {}

    void visit_node(node &n) override {
      n.set_dirty();
      n.traverse(*this);
    }

    void visit_value(value_impl &v) override { v.traverse(*this); }

    gl_scheduler &sched_;
  } t{*this};

  t.visit_value(img);
}

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

void gl_scheduler::op_clear_tex_integer::execute(gl_scheduler &sched) {
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

void gl_scheduler::op_memory_barrier::execute(gl_scheduler &sched) {
  gl::MemoryBarrier(barriers);
}

void gl_scheduler::op_upload_uniform::execute(gl_scheduler &sched) {
  gl_buffer_slice slice;
  sched.uploadbuf_.upload(src, size, g_gl_impl_limits.uniform_buffer_alignment,
                          sched.frame_id + 1, slice);
  bs->ubo[slot] = slice.obj;
  bs->ubo_sizes[slot] = slice.size;
  bs->ubo_offsets[slot] = slice.offset;
}

void gl_scheduler::op_dispatch_compute::execute(gl_scheduler &sched) {
  gl::DispatchCompute(node->ws.x, node->ws.y, node->ws.z);
}

void gl_scheduler::op_load_default_bind_state::execute(gl_scheduler &sched) {
  bs->reset();
}

void gl_scheduler::op_bind_shader_resources::execute(gl_scheduler &sched) {
  if (not_empty(mask & gl_shader_resource_state_mask::vertex_buffers)) {
    gl::BindVertexBuffers(0, (GLsizei)bs->vbo.size(), bs->vbo.data(),
                          bs->vbo_offsets.data(), bs->vbo_strides.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::uniform_buffers)) {
    gl::BindBuffersRange(gl::UNIFORM_BUFFER, 0, (GLsizei)bs->ubo.size(),
                         bs->ubo.data(), bs->ubo_offsets.data(),
                         bs->ubo_sizes.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::texture_units)) {
    gl::BindTextures(0, (GLsizei)bs->textures.size(), bs->textures.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::image_units)) {
    gl::BindImageTextures(0, (GLsizei)bs->images.size(), bs->images.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::shader_storage_buffers)) {
    gl::BindBuffersRange(gl::SHADER_STORAGE_BUFFER, 0, (GLsizei)bs->ssbo.size(),
                         bs->ssbo.data(), bs->ssbo_offsets.data(),
                         bs->ssbo_sizes.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::sampler_objects)) {
    gl::BindSamplers(0, (GLsizei)bs->samplers.size(), bs->samplers.data());
  }
  if (not_empty(mask & gl_shader_resource_state_mask::index_buffer)) {
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, bs->ibo);
  }
}

void gl_scheduler::op_draw::execute(gl_scheduler &sched) {
  if (node->cmd.indexed) {
    size_t s = 4;
    if (index_type == gl::UNSIGNED_INT)
      s = 4;
    else if (index_type == gl::UNSIGNED_SHORT)
      s = 2;
    gl::DrawElementsInstancedBaseVertexBaseInstance(
        prim_type, node->cmd.draw_indexed.index_count, index_type,
        (const void *)(node->cmd.draw_indexed.first_index * s),
        node->cmd.draw_indexed.instance_count,
        node->cmd.draw_indexed.vertex_offset,
        node->cmd.draw_indexed.first_instance);
  } else {
    gl::DrawArraysInstancedBaseInstance(
        prim_type, node->cmd.draw.first_vertex, node->cmd.draw.vertex_count,
        node->cmd.draw.instance_count, node->cmd.draw.instance_count);
  }
}

/*void gl_scheduler::op_bind_draw_state::execute(gl_scheduler &sched)
{
        gl_bind_draw_state(*state, mask);
}
*/

void gl_scheduler::op_bind_compute_pipeline::execute(gl_scheduler &sched) {
  gl::UseProgram(prog->gl_program);
}

void gl_scheduler::op_bind_graphics_pipeline::execute(gl_scheduler &sched) {
  gl::BindVertexArray(prog->gl_vao);
  gl::UseProgram(prog->gl_program);
  gl_bind_draw_state(prog->draw_state);
}
