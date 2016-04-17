#pragma once
#include <gsl.h>

#include "scheduler.hpp"
#include "buffer.hpp"
#include "cast_node.hpp"
#include "clear_node.hpp"
#include "compute_node.hpp"
#include "draw_node.hpp"
#include "gl_framebuffer.hpp"
#include "image.hpp"
#include "texture_cache.hpp"
#include "upload_buffer.hpp"

enum class gl_surface_kind {
  texture,
  screen,
  // TODO: presentable image
};

struct gl_surface {
  gl_surface(gl_surface_kind kind) : kind_{kind} {}
  gl_surface_kind kind_;
};

struct gl_texture_surface : public gl_surface {
  gl_texture_surface(gl_texture &tex, unsigned index = 0,
                     unsigned mip_level = 0)
      : gl_surface{gl_surface_kind::texture}, tex_{tex}, index_{index},
        mip_level_{mip_level} {}

  gl_texture &tex_;
  unsigned index_;
  unsigned mip_level_;
};

struct gl_screen_surface : public gl_surface {
  gl_screen_surface() : gl_surface{gl_surface_kind::screen} {}
};

struct gl_bind_state {
  std::vector<GLuint> vbo;
  std::vector<GLintptr> vbo_offsets;
  std::vector<GLsizei> vbo_strides;
  std::vector<GLuint> textures;
  std::vector<GLuint> samplers;
  std::vector<GLuint> images;
  std::vector<GLuint> ubo;
  std::vector<GLsizeiptr> ubo_sizes;
  std::vector<GLintptr> ubo_offsets;
  std::vector<GLuint> ssbo;
  std::vector<GLsizeiptr> ssbo_sizes;
  std::vector<GLintptr> ssbo_offsets;
  GLuint ibo;

  void reset() {
    ibo = 0;
    //ibo_type = gl::UNSIGNED_INT;
    std::fill(vbo.begin(), vbo.end(), 0);
    std::fill(vbo_offsets.begin(), vbo_offsets.end(), 0);
    std::fill(vbo_strides.begin(), vbo_strides.end(), 0);
    std::fill(textures.begin(), textures.end(), 0);
    std::fill(samplers.begin(), samplers.end(), 0);
    std::fill(images.begin(), images.end(), 0);
    std::fill(ubo.begin(), ubo.end(), 0);
    std::fill(ubo_sizes.begin(), ubo_sizes.end(), 0);
    std::fill(ubo_offsets.begin(), ubo_offsets.end(), 0);
    std::fill(ssbo.begin(), ssbo.end(), 0);
    std::fill(ssbo_sizes.begin(), ssbo_sizes.end(), 0);
    std::fill(ssbo_offsets.begin(), ssbo_offsets.end(), 0);
  }
};

enum class gl_shader_resource_state_mask
{
	texture_units = (1<<0),
	image_units = (1<<1),
	shader_storage_buffers = (1<<2),
	uniform_buffers = (1<<3),
	vertex_buffers = (1<<4),
	index_buffer = (1<<5),
	sampler_objects = (1<<6),
	all = 0xFF
};

ENUM_BIT_FLAGS_OPERATORS(gl_shader_resource_state_mask)

/////////////////////////////////////////////////////
//
struct gl_scheduler : public scheduler {
  // optimize: create an optimized evaluation plan
  //   in: values (gsl::span of pointers)
  //   out: void
  void optimize(std::shared_ptr<value_impl> v);

  // evaluate: render an image onto a surface (image data, screen, GPU texture)
  //   in: image, target surface
  //   out: void
  // side-effects: validate all predecessors of the node
  void evaluate(const image_impl &img, const gl_surface &target);

  // schedule: mark a node (and its successors) dirty
  //   in: value
  void schedule(value_impl &img);

  // evaluation plan = list of commands
  // in each command: list of values that are realized by this command
  // (image_impl, gl_texture, gl_buffer)

  // execution: schedule plans
  // scheduled plans = (plan + pointer)
  // when schedule() is called:
  //
  //  reset pointers to beginning of command list

  enum class gl_opcode {
    begin_frame,
    clear_fbo_float,
    clear_fbo_integer,
    clear_tex_float,
    clear_tex_integer,
    copy_tex_host_device,
    copy_tex_device_device,
    memory_barrier,
    upload_uniform,
    upload_buffer, // dynamic size
    load_default_bind_state,
    load_bind_state,
	bind_resources,
	bind_graphics_pipeline,
	bind_compute_pipeline,
    dispatch_compute,
	draw,
    host_operation,
    end_frame
  };

  struct op {
    gl_opcode opcode;
    virtual void execute(gl_scheduler &sched) {}
  };

  struct op_clear_fbo_float : public op {
    GLuint fbo;
    GLbitfield bufmask;
    float clear_color[4];
    float depth;
    void execute(gl_scheduler &sched) override;
  };

  struct op_clear_fbo_integer : public op {
    GLuint fbo;
    GLbitfield bufmask;
    int clear_color[4];
    void execute(gl_scheduler &sched) override;
  };

  struct op_clear_tex_float : public op {
    gl_texture *target;
    unsigned level;
    float clear_color[4];
    void execute(gl_scheduler &sched) override;
  };

  struct op_clear_tex_integer : public op {
    gl_texture *target;
    int clear_color[4];
    void execute(gl_scheduler &sched) override;
  };

  struct op_copy_texture_host_device : public op {
    uint8_t *src; // source data, linear layout
    size_t size;
    gl_texture *dest; // destination texture
    unsigned level;
    void execute(gl_scheduler &sched) override;
  };

  struct op_copy_texture_device_device : public op {
    // same size, same format
    gl_texture *src; // source texture
    unsigned srclevel;
    gl_texture *dest; // destination texture
    unsigned dstlevel;
    void execute(gl_scheduler &sched) override;
  };

  struct op_memory_barrier : public op {
    GLbitfield barriers;
    void execute(gl_scheduler &sched) override;
  };

  struct op_upload_uniform : public op {
    uint8_t *src;
    size_t size;
    unsigned slot;
	gl_bind_state* bs;
    void execute(gl_scheduler &sched) override;
  };

  struct op_dispatch_compute : public op {
	  compute_node* node;
    void execute(gl_scheduler &sched) override;
  };

  struct op_load_default_bind_state : public op {
	  gl_bind_state* bs;
	  void execute(gl_scheduler &sched) override;
  };

  struct op_bind_shader_resources : public op {
	  gl_bind_state* bs;
	  gl_shader_resource_state_mask mask;
	  void execute(gl_scheduler &sched) override;
  };

  struct op_bind_graphics_pipeline : public op {
	  graphics_pipeline_program* prog;
	  gl_draw_state_mask mask;
	  void execute(gl_scheduler &sched) override;
  };

  struct op_bind_compute_pipeline : public op {
	  compute_pipeline_program* prog;
	  void execute(gl_scheduler &sched) override;
  };

  // bind dynamic draw state
  /*struct op_bind_draw_state : public op {
	  gl_draw_state* state;
	  void execute(gl_scheduler &sched) override;
  };*/

  struct op_draw : public op {
	  draw_node* node;
	  GLenum prim_type;
	  GLenum index_type;
	  void execute(gl_scheduler &sched) override;
  };

  texture_cache texcache_;
  std::vector<std::unique_ptr<gl_texture>> texres_;
  std::vector<std::unique_ptr<gl_buffer>> bufres_;
  std::vector<std::unique_ptr<gl_framebuffer>> fbores_;
  upload_buffer uploadbuf_;
  std::unique_ptr<gl_bind_state> bind_state;
  uint64_t frame_id;
};
