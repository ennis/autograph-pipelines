#pragma once
#include <gsl.h>

#include "buffer.hpp"
#include "cast_node.hpp"
#include "clear_node.hpp"
#include "compute_node.hpp"
#include "draw_node.hpp"
#include "image.hpp"
#include "upload_buffer.hpp"
#include "texture_cache.hpp"
#include "gl_framebuffer.hpp"

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

/////////////////////////////////////////////////////
//
struct gl_scheduler {
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
  void schedule(const value_impl &img);

  // evaluation plan = list of commands
  // in each command: list of values that are realized by this command
  // (image_impl, gl_texture, gl_buffer)

  // execution: schedule plans
  // scheduled plans = (plan + pointer)
  // when schedule() is called:
  //
  //  reset pointers to beginning of command list

  using resource_id = uint32_t;

  // associated with a graph node
  struct sched_node {
    // sched_node() = default;

    bool dirty = false;
    // associated value and node
    std::weak_ptr<value_impl> val;
    // output image resource index
    std::vector<resource_id> imgout;
    // output buffer resource index
    std::vector<resource_id> bufout;
    // framebuffer object (optional, for draw nodes only)
    // GLuint fbo;
  };

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
    upload_buffer,  // dynamic size
    dispatch_compute,
    host_operation,
    end_frame
  };

  struct op {
    gl_opcode opcode;
	virtual void execute(gl_scheduler& sched) {}
  };
  
  struct op_clear_fbo_float : public op {
	  GLuint fbo;
	  GLbitfield bufmask;
	  float clear_color[4];
	  void execute(gl_scheduler& sched) override;
  };

  struct op_clear_fbo_integer : public op {
	  GLuint fbo;
	  GLbitfield bufmask;
	  int clear_color[4];
	  void execute(gl_scheduler& sched) override;
  };

  struct op_clear_tex_float : public op {
    gl_texture* target;
	unsigned level;
    float clear_color[4];
	void execute(gl_scheduler& sched) override;
  };

  struct op_clear_tex_integer : public op {
    gl_texture* target;
    int clear_color[4];
	void execute(gl_scheduler& sched) override;
  };

  struct op_copy_texture_host_device : public op {
    uint8_t* src; // source data, linear layout
    size_t size;
    gl_texture dest; // destination texture
	void execute(gl_scheduler& sched) override;
  };

  struct op_copy_texture_device_device : public op {
    // same size, same format
    gl_texture* src; // source texture
    gl_texture* dest; // destination texture
	void execute(gl_scheduler& sched) override;
  };

  struct op_memory_barrier : public op {
	  void execute(gl_scheduler& sched) override;
  };

  struct op_upload_uniform : public op
  {
    uint8_t* src;
    size_t size;
    gl_buffer_slice* dest;
	void execute(gl_scheduler& sched) override;
  };

  struct op_dispatch_compute : public op 
  {
    // buffer bindings (GPU buffer slices)
    // image bindings (textures)
    std::vector<gl_buffer_slice*> uniforms_;
    std::vector<gl_buffer_slice*> shader_storage_;
    std::vector<gl_buffer_slice*> ssbo_;
  };

  void execute_op(op& op);

  texture_cache texcache_;
  std::vector<std::unique_ptr<sched_node>> nodes_;
  std::vector<std::unique_ptr<gl_texture>> texres_;
  std::vector<std::unique_ptr<gl_buffer>> bufres_;
  std::vector<std::unique_ptr<gl_framebuffer>> fbores_;
  upload_buffer uploadbuf_;
};
