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
    dispatch_compute,
    host_operation,
    end_frame
  };

  struct op {
    gl_opcode opcode;
    virtual void execute() {}
  };

  struct op_clear_tex_float : public op {
    std::shared_ptr<gl_texture> target;
    float clear_color[4];
  };

  struct op_clear_tex_integer : public op {
    std::shared_ptr<gl_texture> target;
    int clear_color[4];
  };

  struct op_copy_texture_host_device : public op {
    std::shared_ptr<uint8_t[]> src; // source data, linear layout
    std::shared_ptr<gl_texture> dest; // destination texture
  };

  struct op_copy_texture_device_device : public op {
    // same size, same format
    std::shared_ptr<gl_texture> src; // source texture
    std::shared_ptr<gl_texture> dest; // destination texture
  };

  struct op_memory_barrier : public op {};

  struct op_upload_uniform 
  {
    std::shared_ptr<uint8_t[]> src;
    std::shared_ptr<gl_buffer_slice> dest;
  };

  struct op_dispatch_compute : public op 
  {
    // buffer bindings (GPU buffer slices)
    // image bindings (textures)
    std::vector<std::shared_ptr<gl_buffer_slice>> uniforms_;
    std::vector<std::shared_ptr<gl_buffer_slice>> shader_storage_;
    std::vector<std::shared_ptr<gl_buffer_slice>> ssbo_;
  };

  texture_cache texcache_;
  std::vector<std::unique_ptr<sched_node>> nodes_;
  std::vector<std::shared_ptr<gl_texture>> texres_;
  std::vector<std::shared_ptr<gl_buffer>> bufres_;
  upload_buffer uploadbuf_;
};
